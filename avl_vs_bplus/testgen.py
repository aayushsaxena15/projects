#!/usr/bin/python
#Arun JVS

import sys, random

#Comment a command if you din't implement it
commands = [
	"I",  #Insert
	"S",  #Search
	"D",  #Delete
	"P",  #Print in sorted order
]


if(len(sys.argv))!=4:
	print("usage: %s numElements inputFile outputFile" % sys.argv[0])
	exit(1)

numElements = int(sys.argv[1])
numCommands = numElements*10

inputFile = open(sys.argv[2], "w")
outputFile = open(sys.argv[3], "w")

elements = range(1, 1+numElements)
available = {i:1 for i in elements}
inserted  = {}

random.seed(hash('~ArgoFishYourself~'))

for i in range(numCommands):
	c = random.choice(commands)
	if(c=="S"):
		e = random.choice(elements)
		inputFile.write("S %d\n" %e)
		outputFile.write("Found\n" if e in inserted else "Not Found\n")
	if(c=="D"):
		if(len(inserted)>0):
			e = random.choice(inserted.keys())
			inputFile.write("D %d\n" %e)
			del inserted[e]
			available[e]=1
		else:
			c="I"
	if(c=="I"):
		if(len(available)>0):
			e = random.choice(available.keys())
			inputFile.write("I %d\n" %e)
			del available[e]
			inserted[e]=1
		else:
			c="P"
	if(c=="P"):
		inputFile.write("P\n")
		outputFile.write(" ".join([str(x) for x in sorted(inserted.keys())])+"\n")
		
inputFile.write("Q\n")
inputFile.close()
outputFile.close()
