import traceback

INPUT_FILE = "first_log.txt"

# From Z1 node datasheet
CURRENT_MA = {
    "CPU" : 10,
    "LPM" : 0.023,
    "DEEP_LPM" : 0, # not used by Z1 nodes
    "Radio_LISTEN" : 18.8,
    "TRANSMIT" : 17.4,
}

STATES = list(CURRENT_MA.keys())


VOLTAGE = 3.0 # assume 3 volt batteries
RTIMER_ARCH_SECOND = 32768

def main():
    node_ticks = {}
    node_total_ticks = {}
    
    with open(INPUT_FILE, "r") as f:
        for line in f:
            if "Energest" not in line:
                continue
            fields = line.split()
            # print(fields)
            try:
                tmp = fields[1].split(":")
                node = int(tmp[1])
                # print("#####################")
            except:
                print("error getting node id")
                continue

            if node not in node_ticks:
                # initialize to zero
                node_ticks[node] = { u : 0  for u in STATES }
                node_total_ticks[node] = 0
                
            for field in fields:
                try:
                    if field in STATES:
                        tick_index = fields.index(field) + 1
                        # print(field, tick_index)
                        ticks = int(fields[tick_index][:-1])
                        node_ticks[node][field] = ticks
                    elif field == "Total_time":
                        tick_index = fields.index(field) + 1
                        # print(field, tick_index)
                        ticks = int(fields[tick_index][:-1])
                        node_total_ticks[node] = int(fields[tick_index][:-1])
                        
                    # print((node_ticks))
                except Exception as ex:
                    
                    print("Failed to process line '{}': {}".format(line, ex))
                    print(traceback.format_exc())
                
                

            # try:
            #     state_index = 3
            #     state = fields[state_index]
                
            #     tick_index = state_index + 1
            #     print(state)
            #     if state not in STATES:
            #         print(state , "#############################")
            #         state = fields[state_index] + " " + fields[state_index+1]
                    
            #         tick_index += 1
            #         if state not in STATES:
            #             # add to the total time
            #             if state == "Total time":
            #                 node_total_ticks[node] += int(fields[tick_index])
            #             continue
            #     # add to the time spent in specific state
            #     ticks = int(fields[tick_index][:-1])
            #     node_ticks[node][state] += ticks
            #     # print((node_ticks))
            # except Exception as ex:
            #     print(traceback.format_exc())
            #     print("Failed to process line '{}': {}".format(line, ex))

    nodes = sorted(node_ticks.keys())
    # print("Reached node ticks #############")
    print(node_total_ticks)
    for node in nodes:
        total_avg_current_mA = 0
        period_ticks = node_total_ticks[node]
        period_seconds = period_ticks # / RTIMER_ARCH_SECOND
        for state in STATES:
            ticks = node_ticks[node].get(state, 0)
            current_mA = CURRENT_MA[state]
            # print(period_ticks, node)
            state_avg_current_mA = ticks * current_mA / period_ticks
            total_avg_current_mA += state_avg_current_mA
        total_charge_mC = period_ticks * total_avg_current_mA # / RTIMER_ARCH_SECOND
        total_energy_mJ = total_charge_mC * VOLTAGE
        print("Node {}: {:.2f} mC ({:.3f} mAh) charge consumption, {:.2f} mJ energy consumption in {:.2f} seconds".format(
            node, total_charge_mC, total_charge_mC / 3600.0, total_energy_mJ, period_seconds))

if __name__ == "__main__":
    main()