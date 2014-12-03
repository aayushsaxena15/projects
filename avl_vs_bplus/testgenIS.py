#!/usr/bin/python
#Arun JVS

import sys, random

if(len(sys.argv))!=4:
	print("usage: %s numElements inputFile outputFile" % sys.argv[0])
	exit(1)

numElements = int(sys.argv[1])
numCommands = numElements*2

inputFile = open(sys.argv[2], "w")
outputFile = open(sys.argv[3], "w")

random.seed(hash('~ArgoFishYourself~'))

elements = range(1, 1+numElements)
inserted = {}
searches = range(1, 1+numElements)

random.shuffle(elements)
random.shuffle(searches)

for i in range(numElements):
	inputFile.write("I %d\n" % elements[i])
	inserted[elements[i]]=1
	inputFile.write("S %d\n" % searches[i])
	outputFile.write("Found\n" if searches[i] in inserted else "Not Found\n")

inputFile.write("Q\n")
inputFile.close()
outputFile.close()
