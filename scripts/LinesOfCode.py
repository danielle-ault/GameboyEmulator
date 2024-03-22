import os

src = "../src/"
include = "../include/"

def main():
	totalLines: int = 0
	totalLines += countLinesInFolder(include)
	print()
	totalLines = countLinesInFolder(src)

	print()
	print("Total: " + str(totalLines))


def countLines(file):
	numLines = len(file.readlines())
	return numLines

def countLinesInFolder(folder: str):
	totalLines = 0;

	for filename in os.listdir(folder):
		# fullPath = os.fsdecode(filename)
		with open(folder+ filename, 'r') as file:
			numLines = countLines(file)
			output = filename + ": " + str(numLines)
			print(output)
			totalLines = totalLines + numLines
	
	return totalLines


main()