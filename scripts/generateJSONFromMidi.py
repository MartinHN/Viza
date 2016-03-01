
import os
import glob
import midi
import json

import Orange

midiPath = "/Users/mhermant/Documents/Work/Datasets/Dani/Midi/motown.mid";
folderOut = "/Users/mhermant/Documents/Work/Datasets/Dani/VizaMidi";
midiFiles = glob.glob(midiPath);




def getPatterns(f):

	gl = midi.read_midifile(f)
	gl.make_ticks_abs();
	tick2bar = 1.0/(4.0*gl.resolution)
	res = [{}]
	curPattern = 0;
	startTick = 0;
	patternLength = 2
	localReso = 16.0

	for e in gl[1]:
		if(midi.NoteOnEvent.is_event(e.statusmsg)):
			
			if e.tick*tick2bar - startTick>=patternLength:
				curPattern+=1
				startTick = curPattern * patternLength
				res += [{}]

			if not str(e.pitch) in res[curPattern]:
				res[curPattern][str(e.pitch)] = []
			res[curPattern][str(e.pitch)] += [ int(round(localReso*(e.tick*tick2bar - startTick)))];

	# for r in range(len(res)):
	# 	for d in res[r]:
	# 		res[r][d] = sorted(set(res[r][d]))
	return res




def createBinary(p):
	curP = {}
	for t in p:
		curP[t] = [ 1  if x in p[t] else 0 for x in range(32) ]
	return curP


def hashRythm(p):
	binary ={}
	for b in p:
		binary[b] = 0;
		for bi in range(len(p[b])):
			binary[b]+= p[b][bi] *pow(2,bi)
	return binary

def getAllKeys(e):
	allKeys = []
	for f in e:
		for p in e[f]:
			allKeys+= p

	return sorted(set(allKeys))






patternDict = {}
eventDict= {}
binarizedDict = {}

for f in midiFiles:
	patternDict[f] = getPatterns(f)
	
for f in patternDict:
	eventDict[f] = []
	for p in patternDict[f]:
		eventDict[f]+= [createBinary(p)];

for f in patternDict:
	binarizedDict[f] = []
	for p in eventDict[f]:
		binarizedDict[f] += [hashRythm(p)];




allKeys = getAllKeys(eventDict)


for genre in eventDict:

	for patt in eventDict[genre]:
		for k in allKeys:
			if k in patt:

				print (patt)
			
data = Orange.data.Table("lenses")

domainList = []
shape = Orange.data.DiscreteVariable("shape", ["circle", "square", "oval"])
for k in allKeys:
	for i in range(32):
		domainList += [ Orange.data.DiscreteVariable(str(k)+"_"+str(i),["0","1"])]

domain = Orange.data.Domain(domainList)
print (allKeys)


# print patterns
# print events
# print binarized





