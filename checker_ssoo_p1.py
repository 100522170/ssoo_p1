'''
Operating Systems - Laboratory 1 - System Calls

This program verifies that the format of the laboratory assignment is correct (follows naming specifications and is properly compressed).

'''
import subprocess
import os
import sys
import shutil

def print_format():
    print("Expected format for zip: ssoo_p1_AAAAA_BBBBB_CCCCC.zip")
    print("Expected format for authors.txt: NIA1,LastNames,Name(s)")


if(__name__=="__main__"):
    '''
    Main function of the application. Gets the file passed as an argument. Decompresses it, checks its format and finally runs the tests.
    '''

    if shutil.which("unzip") is None:
        print("Error: The 'unzip' command is not installed on the system.")
        sys.exit(1)

    # We check that a file has been passed as an argument
    if not len(sys.argv) == 2:
        print('Usage: python {} <zip file>'.format(sys.argv[0]))
        print_format()
    else:
        print('CHECKER: verifying', sys.argv[1])
        inputFile = sys.argv[1]
        
        # We check that the file exists
        if not os.path.isfile(inputFile):
            print("The file", inputFile, "does not exist")
            sys.exit(0)
    
        # We check the file name format
        tokens=inputFile.replace(".zip","")
        tokens=tokens.split("_")
        if len(tokens) != 3 and len(tokens) != 4 and len(tokens) != 5:
            print("Incorrect file name format")
            print_format()
            sys.exit(0)
            
        ssoo=tokens[0]
        p1=tokens[1]
        u1=tokens[2]
        u2=""
        u3=""
        if len(tokens)>3:
            u2=tokens[3]
        if len(tokens)>4:
            u3=tokens[4]
        if not (ssoo == "ssoo" and p1 == "p1"):
            print("Incorrect file name format")
            print_format()
            sys.exit(0)

        print("CHECKER: NIA 1:",u1, "NIA 2:", u2, "NIA 3:", u3)

        print('CHECKER: decompressing')

        # We decompress the file into the temporary folder
        subprocess.call(["mkdir", "-p", "./check"])
        tempFolder="./check"
        zipresult=subprocess.call(["unzip","-j",inputFile,"-d",tempFolder])
        if not zipresult == 0:
            print("Error decompressing the zip file")
            sys.exit(0)

        # We check that the authors file exists
        autoresPath = tempFolder+"/authors.txt"
        if not os.path.isfile(autoresPath):
            print("The file authors.txt does not exist in the decompressed folder")
            sys.exit(0)
    
        print("CHECKER: verifying format of authors.txt")
        try:
            with open(autoresPath, "r", encoding="utf-8", errors="ignore") as f:
                lines = f.readlines()
                
                if not lines:
                    print("Error: authors.txt is empty")
                    sys.exit(0)

                for linea_num, line in enumerate(lines, 1):
                    line = line.strip()
                    if not line: 
                        continue 

                    parts = line.split(",")
                    print("Line {} {}".format(linea_num, line))
                    
                    if len(parts) != 3:
                        print(f"Error format authors.txt (Line {linea_num}): '{line}'")
                        print_format()
                        sys.exit(0)
                    
                    nia, apellidos, nombre = parts[0].strip(), parts[1].strip(), parts[2].strip()

                    # Verify that NIA is a number
                    if not nia.isdigit():
                        print(f"Error format authors.txt (Line {linea_num}): The NIA '{nia}' is not valid.")
                        print_format()
                        sys.exit(0)

                    # Verify that last names and name have content
                    if not apellidos or not nombre:
                        print(f"Error format authors.txt (Line {linea_num}): Missing last names or name.")
                        print_format()
                        sys.exit(0)

        except Exception as e:
            print(f"Error reading authors.txt: {e}")
            sys.exit(0)

        # Compile
        makeres=subprocess.call(["make"], cwd=tempFolder)
        #makeres=subprocess.call(["make"])
        if not makeres == 0:
            print("Compilation error")
            sys.exit(0)
        else:
            print("Compilation successful")
