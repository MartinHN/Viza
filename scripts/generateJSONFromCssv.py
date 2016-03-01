
import os
import json



csvPath = "/Users/mhermant/Documents/Work/Datasets/Dani/PAD_distances_revised";
folderOut = "/Users/mhermant/Documents/Work/Datasets/Dani/Viza";
csvFile = open(csvPath , "r");
lines = csvFile.readlines();



def getStyleForIdx(idx):
	if idx < 28:
		return "motown"
	elif idx <	162:
		return "daftpunk"
	elif idx <	206:
		return "garagehouse"
	elif idx < 262:
		return "newjackhouse"


fileNum = 0;
for l in lines :
	jsonOut = { "metadata": {"duration":1} , "values":{},"Class":{"style" : getStyleForIdx(fileNum)}}
	cList =  l.split(" ")
	i =0 ;

	if(fileNum==0):
		lineSize = len(cList)
	else:
		assert len(cList) == lineSize 

	for e in cList[1:]:
		jsonOut["values"][str(i).zfill(3)] = {"dist":float(e)}
		i+=1;
	jsonPath = os.path.join(folderOut,str(fileNum).zfill(3)+".json")
	with open(jsonPath,"w") as jsonF:
		json.dump(jsonOut,jsonF)
	fileNum += 1;



csvFile.close();