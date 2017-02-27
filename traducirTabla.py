dicc = {"planos":[],"instancias":[],"Vo":[] ,"%dHeurPlanos":[] , "%dHeur":[] , "tHeur (seg)":[]}

instancias = []

hacer = 0
ins = False # 1
Vo = False # 2
planos = False # 3

with open("tabla","r") as f:
	lines = f.readlines()
	for line in lines:
		shavedLine = line.replace("\n","").replace("\t","").replace("\r","").replace(" ","")
		if shavedLine == "Instancia":
			hacer = 1
			continue
		if shavedLine == "|V|":
			hacer = 0
			continue
		if shavedLine == "Vo":
			hacer = 2
			continue
		if shavedLine == "Vhp":
			hacer = 3
			continue
		if (hacer == 1):
			dicc["instancias"].append(shavedLine)
		if (hacer == 2):
			dicc["Vo"].append(int(shavedLine))
		if (hacer == 3):
			dicc["planos"].append(int(shavedLine))


arreglo = [[dicc["instancias"][i],dicc["Vo"][i],dicc["planos"][i]] for i in range(len(dicc["instancias"]))]
path = "instanciasPRPP/ALBAIDA/"

import subprocess
from timeit import timeit

espacio = "   \t"
espacioHeader = "\t"

with open("tablaALBAIDA.txt","w") as f:
	f.write("Instancia"+espacioHeader + "Vo"+espacioHeader + "%dHeurPlano"+espacioHeader + "%dHeur"+espacioHeader + "tHeur(seg)\n")
	print("Cosas")
	for [instancia,vo,vhp] in arreglo:
		desvVHP = 0
		desvHeur = 0

		path2 = path + instancia + "NoRPP"
		print(path2)
		solucion = 0
		time = timeit(stmt = "subprocess.call(['./heuristica','"+ path2 +"'])",setup="import subprocess", number = 1)
		with open(path2+"_salida.txt") as t:
			lines = t.readlines()
			line = lines[0].replace("\n","")
			solucion = int(line)
		if (vo != 0):
			desvVHP = 100*((vo-vhp)/vo)
			desvHeur = 100*((vo - solucion)/vo)
		f.write(instancia + espacio + str(vo) + espacio \
				 + str.format("{0:.03f}",desvVHP)+ espacio \
				 + str.format("{0:.03f}",desvHeur) + espacio \
				 + str.format("{0:.03f}",time) + "\n")