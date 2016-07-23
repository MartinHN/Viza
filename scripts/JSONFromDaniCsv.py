import json

file = '../exampleDani/mo-gar-njh-chh_coordinates_PAD_SAD_EUC.csv'
outFolder = '../exampleDani/ViZa/'



with open(file,'r') as f:
	headers = f.readline()
	names = [x for x in headers.split(',') if x not in['','\n']]
	res = {}
	currentStyle = ""
	sIdx = 0;
	for l in f:
		v = l.split(',')
		if(v[0]!=''):
			currentStyle = v[0]	
		sampleName = currentStyle+'_'+str(sIdx)
		res[sampleName] = {'x':{},'y':{}}
		colIdx = 0;
		for colIdx in range(len(names)):
			res[sampleName]['x'][names[colIdx]] = float(v[colIdx*2+1])
			res[sampleName]['y'][names[colIdx]] = float(v[colIdx*2+2])
		sIdx+=1
		
		jsonout = { "metadata": {"duration":1} , "values":res[sampleName],"Class":{"style" :currentStyle}}
		with open(outFolder+'/'+str(sIdx)+'.json' ,'w') as f:
				json.dump(jsonout,f)

	


