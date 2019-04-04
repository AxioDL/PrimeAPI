from Stream import *
from Mangle import *
from DolFile import *
from PreplfFile import *
import glob
import os
import re
import subprocess
import sys

# Environment
primeApiRoot = os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "\\..")
dolphinRoot = ""
cwRoot = ""
cwToolsDir = ""
compilerPath = ""
linkerPath = ""

# Configuration
projDir = ""
buildDir = ""
moduleName = "Mod"
bootstrapHookTarget = "PPCSetFpIEEEMode"
bootstrapSectionIdx = 2
bootstrapAddr = 0x80002800
outFile = ""
dolFile = DolFile()
dolPatches = []
verbose = False
buildDebug = False

def get_filename(sourceFile):
	return os.path.splitext( os.path.split(sourceFile)[1] )[0]

def get_extension(sourceFile):
	return os.path.splitext(sourceFile)[1]

def parse_commandline():
	global projDir, buildDir, bootstrapHookTarget, bootstrapSectionIdx, bootstrapAddr, moduleName, outFile, verbose, buildDebug
	
	if len(sys.argv) < 3:
		print("Please specify a project directory and a dol to link to!")
		return False
	
	# Set project directory
	projDir = sys.argv[1]
	if projDir.endswith("\\") or projDir.endswith("/"):
		projDir = projDir[:-1]
	
	buildDir = "%s\\build" % projDir
	
	# Read DOL
	dolFile.read(sys.argv[2])
	
	if dolFile.buildVersion >= 3:
		print("The specified dol file belongs to a Wii version of the game. The Wii versions are currently not supported.")
		return False
	
	if not dolFile.load_symbols(primeApiRoot + "\\symbols"):
		return False
		
	# Check other arguments
	argIdx = 3
	
	while argIdx < len(sys.argv):
		arg = sys.argv[argIdx]
		
		if arg == "-debug":
			buildDebug = True
		
		elif arg == "-bootstrap_hook":
			bootstrapHookTarget = sys.argv[argIdx + 1]
			argIdx += 1

		elif arg == "-bootstrap_section":
			bootstrapSectionIdx = int(sys.argv[argIdx + 1])
			argIdx += 1

			# Sections 0 and 1 are in use by the game. 2 through 6 can be used by mods.
			if bootstrapSectionIdx < 2 or bootstrapSectionIdx > 6:
				print("An invalid bootstrap section index was provided. Please specify an index between 2 and 6.")
				return False
		
		elif arg == "-bootstrap_address":
			bootstrapAddr = int(sys.argv[argIdx + 1], 16)
			argIdx += 1

		elif arg == "-m":
			moduleName = sys.argv[argIdx + 1]
			argIdx += 1
			
		elif arg == "-o":
			outFile = sys.argv[argIdx + 1]
			argIdx += 1
		
		elif arg == "-v":
			verbose = True
			
		argIdx += 1
		
	# Set default values for some arguments
	if not outFile:
		outFile = "%s\\%s.rel" % (buildDir, moduleName)
	else:
		moduleName = get_filename(outFile)
	
	return True
	
def get_object_path(sourceFile):
	return "%s\\%s.o" % (buildDir, get_filename(sourceFile))

def generate_scoped_decl(declText, declType):
	# This allows us to forward-declare scoped classes and functions which are scoped inside of a namespace or a class. While
	# normally you wouldn't be allowed to declare a namespace with the same name as a class, in this case ApplyCodePatches is
	# compiled separately from the other modules and therefore doesn't know of any name conflicts. Using namespaces like this
	# causes the compiler to generate an identical symbol to the class member, which can then later be resolved by the linker.
	scopes = split_scopes(declText)
	baseName = scopes[-1]
	del scopes[-1]
	outText = ""
	
	for scope in scopes:
		outText += "namespace %s { " % scope
	
	outText += "%s %s;" % (declType, baseName)
	
	for scope in scopes:
		outText += " }"
	
	outText += '\n'
	return outText
	
def generate_patch_code():
	template = open("%s\\script\\ApplyCodePatches_Template.cpp" % primeApiRoot, "r")
	code = template.read()
	template.close()
	
	# Find memory locations that need patches
	sortedPatches = sorted(dolPatches, key=lambda patch: patch['address'])
	classDecls = ""
	funcDecls = ""
	funcCode = ""
	usedSymbols = []
	usedSymbols = []
	
	for patch in dolPatches:
		address = patch['address']
		symbol = patch['symbol']
		type = patch['type']
		
		# For function pointers, use static_cast to ensure the correct overload is used
		argsStart = symbol.find('(')
		
		if argsStart != -1:
			argsEnd = symbol.rfind(')')
			assert(argsEnd != -1)
			
			funcName = symbol[0:argsStart]
			params = symbol[argsStart+1:argsEnd]
			symbolRef = "static_cast<void (*)(%s)>(&%s)" % (params, funcName)

		else:
			symbolRef = "&%s" % symbol
		
		# Generate code for this patch
		if type == R_PPC_REL24:
			patchCode = '\n\tRelocate_Rel24((void*) 0x%08X, %s);' % (address, symbolRef)
		
		elif type == R_PPC_ADDR32:
			patchCode = '\n\tRelocate_Addr32((void*) 0x%08X, %s);' % (address, symbolRef)
			
		else:
			continue
		
		if not symbol in usedSymbols:
			paramsStart = symbol.find('(')
			isFunction = paramsStart != -1
			
			if isFunction:
				# Add forward decl for classes referenced in the function parameters
				paramsEnd = symbol.rfind(')')
				params = split_params(symbol[paramsStart+1:paramsEnd])
				
				for param in params:
					paramStr = ''.join([chr for chr in param if not chr in "*&"])
					paramWords = paramStr.split(' ')
					
					for word in paramWords:
						if word and word != 'const' and word != 'unsigned' and word != 'signed':
							# this is probably our type name!
							if not is_basic_type(word):
								classDecls += generate_scoped_decl(word, "class")
							break
				
				funcDecls += generate_scoped_decl(symbol, "void")
				usedSymbols.append(symbol)
		
		funcCode = funcCode + patchCode
	
	cpptext = code % (classDecls + funcDecls, funcCode)
	
	# Save file
	cppname = "%s\\ApplyCodePatches.cpp" % buildDir
	out = open(cppname, "w")
	out.write(cpptext)
	out.close()
	return cppname

def parse_code_macros(sourcePath):
	# Parse file, look for PATCH_SYMBOL macros
	# This implementation leaves a bit to be desired - namely, it doesn't account for commented-out
	# code or #ifdef'd out code, and won't correctly mangle templates that have default parameters
	sourceFile = open(sourcePath, 'r')
	regex = re.compile("^PATCH_SYMBOL\((.*)\((.*)\)(.*),(.*)\((.*)\)(.*)\)")
	pos = 0
	
	while True:
		line = sourceFile.readline()
		if not line: break
		
		# Replace spaces except ones denoting const parameters
		line = line.strip()
		strippedLine = ""
		
		for chrIdx in range(0, len(line)):
			chr = line[chrIdx]
			
			if chr != ' ':
				strippedLine += chr
			
			else:
				if chrIdx >= 5 and line[chrIdx - 5 : chrIdx] == "const":
					# Verify this const precedes a type name by verifying the next non-whitespace character is not a comma
					for chrIdx2 in range(chrIdx, len(line)):
						if line[chrIdx2] != ' ':
							if line[chrIdx2] != ',':
								strippedLine += chr
							break
		
		match = regex.search(strippedLine)
		
		if match is not None:
			origSymbol = "%s(%s)%s" % (match.group(1), match.group(2), match.group(3))
			patchSymbol = "%s(%s)%s" % (match.group(4), match.group(5), match.group(6))
			pos = match.endpos
			
			dolPatches.extend( dolFile.generate_patches(mangle(origSymbol), patchSymbol) )
		
def compile_object(sourcePath, outPath):
	# Create output directory
	if not os.path.exists(outPath):
		os.makedirs(outPath)
	
	parse_code_macros(sourcePath)
	
	# Check extension
	print("Compiling %s" % sourcePath)
	ext = get_extension(sourcePath)
	
	if ext == ".cpp": lang = "c++"
	elif ext == ".c": lang = "c"
	
	else:
		print("%s: unrecognized extension (%s)" % (sourcePath, ext))
		return False
	
	# Compile
	includeDirs = ["-i-", # mark the include directories as system paths
				   "-I%s\\include" % primeApiRoot,
				   "-I%s\\include" % dolphinRoot,
				   "-I%s\\PowerPC_EABI_Support\\Runtime\\Inc" % cwRoot,
				   "-ir %s\\PowerPC_EABI_Support\\Msl\\MSL_C" % cwRoot]
	
	if lang == "c++":
		includeDirs.append("-ir %s\\PowerPC_EABI_Support\\Msl\\MSL_C++" % cwRoot)
	
	args = [compilerPath,
			"-align powerpc",
			"-enum int",
			"-proc gekko",
			"-fp hardware",
			"-lang %s" % lang,
			"-Cpp_exceptions off",
			"-sdata 0",
			"-sdata2 0",
			"-O",
			" ".join(includeDirs),
			"-c %s" % sourcePath,
			"-o %s" % get_object_path(sourcePath)]
	
	# Run compiler
	command = ' '.join(args)
	if verbose:
		print(">>> %s" % command)
	ret = subprocess.call(command)
	return ret == 0

def link_objects(objList):
	print("Linking objects")
	
	args = [linkerPath,
			' '.join(objList),
			"-unused",
			"-r",
			"-fp hardware",
			"-map %s\\%s.map" % (buildDir, moduleName),
			"-m _prolog",
			"-lcf %s\\include\\dolphin\\eppc.lcf" % dolphinRoot,
			"-o %s\\%s.preplf" % (buildDir, moduleName)]
	
	# Run linker
	command = ' '.join(args)
	if verbose:
		print(">>> %s" % command)
	ret = subprocess.call(command)
	return ret == 0
	
def convert_preplf_to_rel(preplfPath, outRelPath):
	preplf = PreplfFile(preplfPath)
	rel = OutputStream()
	
	# Initial header info
	rel.write_long(2) # Module ID. Hardcoding 2 here because 0 is the dol and the game already has a module using 1 (NESemu.rel). Should be more robust ideally
	rel.write_long(0) # Next module link - always 0
	rel.write_long(0) # Prev module link - always 0	
	rel.write_long(len(preplf.sections)) # Num sections
	rel.write_long(0) # Section info offset filler
	rel.write_long(0) # Module name offset (our rel won't include the module name so this is staying null)
	rel.write_long(0) # Module name size
	rel.write_long(2) # Module version
	
	# Fetch data needed for the rest of the header
	bssSec = preplf.section_by_name(".bss")
	assert(bssSec != None)
	
	prologSymbol = preplf.symbol_by_name("_prolog")
	if prologSymbol is None:
		prologSymbol = preplf.symbol_by_name("_prolog__Fv")

	epilogSymbol = preplf.symbol_by_name("_epilog")
	if epilogSymbol is None:
		epilogSymbol = preplf.symbol_by_name("_epilog__Fv")

	unresolvedSymbol = preplf.symbol_by_name("_unresolved")
	if unresolvedSymbol is None:
		unresolvedSymbol = preplf.symbol_by_name("_unresolved__Fv")
	
	# Remaining header data
	rel.write_long(bssSec.size)
	rel.write_long(0) # Relocation table offset filler
	rel.write_long(0) # Imports offset filler
	rel.write_long(0) # Imports size filler
	rel.write_byte(prologSymbol['sectionIndex'] if prologSymbol is not None else 0)
	rel.write_byte(epilogSymbol['sectionIndex'] if epilogSymbol is not None else 0)
	rel.write_byte(unresolvedSymbol['sectionIndex'] if unresolvedSymbol is not None else 0)
	rel.write_byte(0) # Padding
	rel.write_long(prologSymbol['value'] if prologSymbol is not None else 0)
	rel.write_long(epilogSymbol['value'] if epilogSymbol is not None else 0)
	rel.write_long(unresolvedSymbol['value'] if unresolvedSymbol is not None else 0)
	rel.write_long(8) # Module alignment
	rel.write_long(8) # BSS alignment
	
	# Section info filler
	sectionInfoOffset = rel.tell()
	
	for section in preplf.sections:
		rel.write_long(0)
		rel.write_long(0)
	
	# Write sections
	sectionInfoList = []
	wroteBss = False
	
	for section in preplf.sections:
		# Sections not in the to-keep list should be nulled out
		info = {}
		info['exec'] = (section.flags & 0x4) != 0
		
		secStart = rel.tell()
		name = section.name
		info['name'] = name
		
		allowedSections = [
			".init",
			".text",
			".ctors",
			".dtors",
			".rodata",
			".data"
		]
		
		isBss = name == ".bss"
		shouldKeep = (name in allowedSections)
		
		if shouldKeep is True:
			info['offset'] = secStart
			rel.write_bytes(section.data)
			rel.write_to_boundary(4)
			info['size'] = rel.tell() - secStart
			
		elif isBss is True:
			info['offset'] = 0
			info['size'] = section.size
			wroteBss = True
			
		else:
			assert(not name or wroteBss is True)
			info['offset'] = 0
			info['size'] = 0
		
		sectionInfoList.append(info)
	
	# Generate imports and write imports section filler
	imports = []
	moduleImports = { 'moduleID': 2, 'relocsOffset': 0 }
	dolImports = { 'moduleID': 0, 'relocsOffset': 0 }
	imports.append(moduleImports)
	imports.append(dolImports)
	
	importsOffset = rel.tell()
	
	for importIdx in range(0, len(imports)):
		rel.write_long(0)
		rel.write_long(0)

	importsSize = rel.tell() - importsOffset
	
	# Write relocations
	relocsOffset = rel.tell()
	relocWriteSuccess = True
	
	for importIdx in range(0, 2):
		imports[importIdx]['relocsOffset'] = rel.tell()
		isDol = imports[importIdx]['moduleID'] == 0
		
		for section in preplf.sections:
			if section.type != EST_RELA or len(section.relocs) == 0:
				continue
			
			symbolSection = section.link
			targetSection = section.targetSecIdx
			
			# Make sure we only write relocations for sections that were written to the file
			sectionInfo = sectionInfoList[targetSection]
			if sectionInfo['offset'] == 0:
				continue
			
			curOffset = 0
			wroteSectionCommand = False
			
			# Parse relocations
			for reloc in section.relocs:
				symbol = preplf.fetch_symbol(symbolSection, reloc['symbolIdx'])
				assert(symbol != None)
				
				# DOL relocations have a section index of 0; internal relocations have a valid section index
				if (symbol['sectionIndex'] == 0) != isDol:
					continue
				
				# This is a valid relocation, so we have at least one - write the "change section" directive
				if not wroteSectionCommand:
					rel.write_short(0)
					rel.write_byte(R_DOLPHIN_SECTION)
					rel.write_byte(targetSection)
					rel.write_long(0)
					wroteSectionCommand = True
			
				offset = reloc['offset'] - curOffset
				
				# Add "nop" directives to make sure the offset will fit
				# NOTE/TODO - not sure if this is actually supposed to be a signed offset - that needs to be verified
				while offset > 0xFFFF:
					rel.write_short(0xFFFF)
					rel.write_byte(R_DOLPHIN_NOP)
					rel.write_byte(0)
					rel.write_long(0)
					offset -= 0xFFFF
					curOffset += 0xFFFF
				
				# Write relocation
				rel.write_short(offset)
				rel.write_byte(reloc['relocType'])
				
				# Internal relocs are easy - just copy data from the ELF reloc/symbol
				if not isDol:
					rel.write_byte(symbol['sectionIndex'])
					rel.write_long(symbol['value']) # this is basically just the section-relative offset to the symbol
				
				# DOL relocs will require looking up the address of the symbol in the DOL
				else:
					symbolName = symbol['name']
					dolSymbolAddr = dolFile.get_symbol(symbolName)
					
					if dolSymbolAddr is None:
						print("Error: Failed to locate dol symbol: %s" % symbolName)
						rel.write_byte(0)
						rel.write_long(0)
						relocWriteSuccess = False
						continue
					
					rel.write_byte(0)
					rel.write_long(dolSymbolAddr)
				
				curOffset += offset
				
		# Write "end" directive
		rel.write_short(0)
		rel.write_byte(R_DOLPHIN_END)
		rel.write_byte(0)
		rel.write_long(0)
	
	# Quit out?
	if not relocWriteSuccess:
		return False
	
	# Write filler values from the header
	rel.goto(0x10)
	rel.write_long(sectionInfoOffset)
	rel.goto(0x24)
	rel.write_long(relocsOffset)
	rel.write_long(importsOffset)
	rel.write_long(importsSize)
	
	rel.goto(sectionInfoOffset)
	
	for section in sectionInfoList:
		# Toggle 0x1 bit on the section offset for sections containing executable code
		offset = section['offset']
		if section['exec'] is True: offset |= 0x1
		
		rel.write_long(offset)
		rel.write_long(section['size'])
		
	rel.goto(importsOffset)
	
	for imp in imports:
		rel.write_long(imp['moduleID'])
		rel.write_long(imp['relocsOffset'])
	
	# Done
	rel.save_file(outRelPath)
	print("Saved REL to %s" % outRelPath)
	return True
	
def compile_rel():
	sourceFiles = []
	sourceFiles.extend( glob.glob("%s\\*.cpp" % projDir) )
	sourceFiles.extend( glob.glob("%s\\*.c" % projDir) )
	sourceFiles.append( "%s\\PowerPC_EABI_Support\\Runtime\\Src\\global_destructor_chain.c" % cwRoot )
	sourceFiles = [file for file in sorted(sourceFiles) if file.find("build\\ApplyCodePatches.cpp") == -1]
	
	# Set up compilation environment
	MWCIncludes = [cwRoot + "\\PowerPC_EABI_Support\\Msl\\MSL_C\\MSL_Common\\Include",
			cwRoot + "\\PowerPC_EABI_Support\\Msl\\MSL_C\\MSL_Common_Embedded\\Include",
			cwRoot + "\\PowerPC_EABI_Support\\Msl\\MSL_C\\PPC_EABI\\Include",
			cwRoot + "\\PowerPC_EABI_Support\\Msl\\MSL_C++\\MSL_Common\\Include",
			cwRoot + "\\PowerPC_EABI_Support\\Runtime\\Inc",
			dolphinRoot + "\\include"]
	
	MWLibraries = [cwRoot + "\\PowerPC_EABI_Support\\MetroTRK",
				   cwRoot + "\\PowerPC_EABI_Support\\Msl\\MSL_C\\PPC_EABI\\LIB",
				   cwRoot + "\\PowerPC_EABI_Support\\Msl\\MSL_C++\\PPC_EABI\\Lib",
				   cwRoot + "\\PowerPC_EABI_Support\\Runtime\\Lib",
				   dolphinRoot + "\\HW2\\lib"]
	
	MWLibraryFiles = ["MSL_C.PPCEABI.bare.H.a",
					  "MSL_C++.PPCEABI.bare.H.a",
					  "fdlibm.PPCEABI.H.a",
					  "sp_mathlib.H.a",
					  "Runtime.PPCEABI.H.a",
					  "TRK_MINNOW_DOLPHIN.a",
					  "TRK_MINNOW_DOLPHIN_1_0.a"]
	
	os.environ["MWCIncludes"]    = ';'.join(MWCIncludes)
	os.environ["MWLibraries"]    = ';'.join(MWLibraries)
	os.environ["MWLibraryFiles"] = ';'.join(MWLibraryFiles)
	
	# Compile/link source files
	objectFiles = []
	
	for sourceFile in sourceFiles:
		if not compile_object(sourceFile, buildDir):
			return False
		else:
			objectFiles.append(get_object_path(sourceFile))
			if verbose: print('')
	
	# Generate patching code
	generatedFile = generate_patch_code()
	genBuildSuccess = compile_object(generatedFile, buildDir)
	
	if not genBuildSuccess:
		return False
	else:
		objectFiles.append(get_object_path(generatedFile))
		if verbose: print('')
		
	# Link
	if not link_objects(objectFiles):
		return False
	
	# We now have a .preplf file in the build folder... final step is to convert it to .rel
	if not convert_preplf_to_rel("%s\\%s.preplf" % (buildDir, moduleName), outFile):
		return False
	
	return True
	
def main():
	# Verify there is a valid installation of the Dolphin SDK and CodeWarrior
	global dolphinRoot, cwRoot
	dolphinRoot = os.getenv("DOLPHIN_ROOT")
	cwRoot = os.getenv("CWFOLDER")

	if dolphinRoot is None:
		print("Error: The DOLPHIN_ROOT environment variable is undefined. BuildModule.py requires an installation of the Dolphin SDK.")
	
	if cwRoot is None:
		print("Error: The CWFOLDER environment variable is undefined. BuildModule.py requires an installation of CodeWarrior version 2.3.3.")
	
	if dolphinRoot is None or cwRoot is None:
		return
	
	# Set globals
	global cwToolsDir, compilerPath, linkerPath
	cwToolsDir = cwRoot + "\\PowerPC_EABI_Tools\\Command_Line_Tools"
	compilerPath = cwToolsDir + "\\mwcceppc.exe"
	linkerPath = cwToolsDir + "\\mwldeppc.exe"
	
	# Parse commandline argments
	if not parse_commandline():
		return
	
	# Apply DOL patches
	shouldContinue = True
	
	if not dolFile.is_patched():
		print("Detected DOL file is unpatched. Applying patch...")
		dolFilename = dolFile.filename
		nameEnd = dolFilename.rfind('.')
		nameBase = dolFilename[0:nameEnd]
		patchedName = "%s_mod.dol" % nameBase
		patchFile = "%s\\script\\DolPatch.bin" % primeApiRoot
		
		shouldContinue = dolFile.apply_patch(bootstrapHookTarget, bootstrapSectionIdx, bootstrapAddr, moduleName, patchFile, patchedName)
		
		if shouldContinue:
			print("Saved patched DOL to %s" % patchedName)
		
	# Compile
	compileSuccess = compile_rel() if shouldContinue else False
	
	print("***** COMPILE %s! *****" % ("SUCCEEDED" if compileSuccess else "FAILED"))

if __name__ == "__main__":
	main()