import netlogo_iterative as nli
import csv 
total_players = nli.launch()

csv_path = '/workspaces/code/sem4git/ccc844/output.csv'


for i in [5,10,30,50,70,90,95]:
    for ii in [1.25, 1.33, 1.5, 1.67, 2]:
        csvfile = open(csv_path, 'a', newline='\n')
        csvwriter = csv.writer(csvfile)
        config = {"num_iterations": 100,
                "ticks_per_iteration": 100,
                "defection_award": ii,
                "initial_cooperation": i}

        endstates = [round(i/total_players*100, 2) for i in nli.run(config)]
        csvdata=[config["defection_award"], config["initial_cooperation"], '-'] + endstates
        print(csvdata)
        csvwriter.writerow(csvdata)
        csvfile.close()

nli.kill()