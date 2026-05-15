# small utility for concatenating files
# the filenames are given in the list "name"
# each file is preceded by "////////// filename" and followed by a newline
# the output file is "output.txt"

name = [ "CMakeLists.txt", "src/msh.h", "src/msh.cpp", "src/main.cpp"]
with open("output.txt", "w") as outfile:
    for fname in name:
        with open(fname) as infile:
            outfile.write("/////////// " + fname + "\n")
            outfile.write(infile.read())
            outfile.write("\n\n")            