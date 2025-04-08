import pynetlogo as pyNetLogo
from tqdm import tqdm
import contextlib
import os
import sys

def launch(
        path_to_netlogo = '/workspaces/code/sem4git/ccc844/NetLogo 6.1.1/app', 
        path_to_model = "/workspaces/code/sem4git/ccc844/NetLogo 6.1.1/app/models/Sample Models/Social Science/Unverified/Prisoner's Dilemma/PD Basic Evolutionary.nlogo"
        ):
    # Launch NetLogo headlessly
    global netlogo
    global devnull
    devnull = open(os.devnull, 'w')
    contextlib.redirect_stderr(devnull)
    netlogo = pyNetLogo.NetLogoLink(gui=False, netlogo_home=path_to_netlogo)
    netlogo.load_model(path_to_model)
    return netlogo.report('count patches')

def run(config: dict = {"num_iterations": 100,
          "ticks_per_iteration": 100,
          "defection_award": 1.59,
          "initial_cooperation": 5}
          ):

    num_iterations = config['num_iterations']
    ticks_per_iteration = config['ticks_per_iteration']
    defection_award = config['defection_award']
    initial_cooperation = config['initial_cooperation']

    #empty array for returning end states
    end_states = []

    #set initial parameters
    netlogo.command('set defection-award ' + str(defection_award))
    netlogo.command('set initial-cooperation ' + str(initial_cooperation))

    # total_players = netlogo.report('count patches') uncomment
    # collapse_count = 0 uncomment

    # for i in range(num_iterations):

    for i in tqdm(range(num_iterations), desc="Iteration", total=num_iterations, initial=1):
        # print("Iteration", i+1, end='') uncomment
        netlogo.command('setup')
        
        for _ in range(ticks_per_iteration):
            netlogo.command('go')
        
        # Count cooperators (color-class 1 = cc, 4 = dc)
        num_cooperators = netlogo.report('count patches with [color-class = 1 or color-class = 4]')
        
        # if num_cooperators == 0: uncomment
        #     collapse_count += 1 uncomment
        # print('     end-state: ', round(num_cooperators/total_players*100, 2) , '% Cooperators',sep='') uncomment
        end_states.append(num_cooperators)
    return end_states
    

def kill():
    netlogo.kill_workspace()