## Title:       bin2array.py
## Author:      Jeroen Venema
## Created:     24/10/2023
## Last update: 24/10/2023

## syntax
## bin2array.py FILENAME
## 

import sys
import time
import os.path

if len(sys.argv) == 1 or len(sys.argv) >4:
  sys.exit('Usage: bin2array.py FILENAME')

if not os.path.isfile(sys.argv[1]):
  sys.exit(f'Error: file \'{sys.argv[1]}\' not found')

if (sys.argv[1])[-3:] == 'bin':
    outputfilebase = os.path.basename(sys.argv[1].removesuffix(".bin"))
else:
    outputfilebase = os.path.basename(sys.argv[1])

outputfilename = outputfilebase + ".h" # create header file
inputfile = open(sys.argv[1], mode="rb")    
outputfile = open(outputfilename, mode="w")

data = inputfile.read()

header = (f"unsigned int {outputfilebase}_bin_len = {len(data)};\n")
header += (f"unsigned char {outputfilebase}_bin [] = {{\n")
outputfile.write(header)


output = ''
itemsperline = 0
itemnumber = 0

for b in data:
    if itemsperline == 0:
       outputfile.write("  ")
    
    output += f'0x{b:02X}'
    itemsperline += 1
    itemnumber += 1

    if itemnumber != len(data):
       output += ', '
    if itemsperline == 16:
       itemsperline = 0
       output += '\n'
       outputfile.write(output)
       output = ''

if len(output):
   outputfile.write(output)
outputfile.write('\n};\n')
inputfile.close()
outputfile.close()
