import netlogo_iterative as nli
import csv
total_players = nli.launch()

csv_path = '/workspaces/code/sem4git/ccc844/output.csv'


# for i in [5,10,30,50,70,90,95]:
for initial_cooperation in [50]:
	# for defector_reward in [1.25, 1.33, 1.5, 1.67, 2]:
	for defector_reward in [1.3325, 1.3350, 1.3375, 1.3400, 1.3425, 1.3450, 1.3475, 1.5955 ,1.596,1.5965, 1.597, 1.5975, 1.598,1.5985, 1.599, 1.5995, 1.601, 1.602,
						  1.603, 1.604, 1.66525, 1.66550, 1.66575, 1.66600, 1.66625, 1.66650, 1.66675, 1.66700, 1.66725]:
		csvfile = open(csv_path, 'a', newline='\n')
		csvwriter = csv.writer(csvfile)
		config = {"num_iterations": 100,
				"ticks_per_iteration": 100,
				"defection_award": defector_reward,
				"initial_cooperation": initial_cooperation}

		endstates = [round(i/total_players*100, 2) for i in nli.run(config)]
		csvdata = [config["defection_award"],
				config["initial_cooperation"], '-'] + endstates
		print(csvdata)
		csvwriter.writerow(csvdata)
		csvfile.close()

nli.kill()
