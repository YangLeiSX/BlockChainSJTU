import os
import argparse

def head(args):
    """Write Head Lines for Graphviz Script

    Args:
        args (argparse.ArgumentParser): Command Line Parameters
    """
    with open(args.output, 'w') as fout:
        fout.write("digraph tree {\n")
        fout.write("\trankdir = LR\n")

def tail(args):
    """Write Tail Lines for Graphviz Script

    Args:
        args (argparse.ArgumentParser): Command Line Parameters
    """
    with open(args.output, 'a') as fout:
        fout.write("}\n")

def gen_node(args, nidx, miner, isNormal=False):
    """Write Node Lines for Graphviz Script

    Args:
        args (argparse.ArgumentParser): Command Line Parameters
        nidx (str): Block ID
        miner (str): Miner of Block
        isNormal (bool, optional): If the Miner is Normal Client. Defaults to False.
    """
    out = "\t\"#{}\\nminer#{}\" [ color = {} ]\n".format(
        nidx, miner, "black" if isNormal else "red")
    with open(args.output, 'a') as fout:
        fout.write(out)

def gen_edge(args, nodes, edge, label):
    """Write Edge Lines for Graphviz Script

    Args:
        args (argparse.ArgumentParser): Command Line Parameters
        nodes (list): All Blocks' ID
        edge (tuple): Nodes of the Edge
        label (list): Client ID who get this Edge
    """
    out = "\t\"#{}\\nminer#{}\" -> \"#{}\\nminer#{}\"\n".format(
        edge[0], nodes[edge[0]], edge[1], nodes[edge[1]])
    with open(args.output, 'a') as fout:
        fout.write(out)

if __name__ == "__main__":
    # Parse all the Command Line Parameters
    parser = argparse.ArgumentParser()
    parser.add_argument("-r","--result", 
                        type=str,
                        default="result.csv",
                        help="存放结果的位置")
    parser.add_argument("-o","--output", 
                        type=str,
                        default="visual/dot_selfish.dot",
                        help="存放脚本的位置")
    parser.add_argument("-g","--graph", 
                        type=str,
                        default="visual/png_selfish.png",
                        help="存放图片的位置")
    parser.add_argument("-s","--selfish", 
                        type=str,
                        default="res_selfish.csv",
                        help="存放自私攻击测试结果的位置")
    parser.add_argument("-t","--timestamp", 
                        type=str,
                        default="0000_000000",
                        help="时间戳")
    parser.add_argument("-d","--display", 
                        action='store_true',
                        default=False,
                        help="是否展示")
    args = parser.parse_args()

    # initilize
    nodes = {}
    edges = {}
    normal = ['65535']
    win = []

    with open(args.result, 'r') as fin:
        for line in fin.readlines():
            # remove '\n'
            line = line.split(',')[:-1]
            user_idx = line[0]
            client_type = line[1]
            line = line[2:]
            if client_type == 'N':
                # Add Normal Users' ID 
                normal.append(user_idx)
            else:
                # Add Selfish Mining Length
                win.append(int(client_type))
            for i in range(0, len(line)-1, 2):
                # Get all Nodes
                if line[i] not in nodes.keys():
                    nodes[line[i]] = line[i+1]
                if i == len(line)-2: 
                    continue
                # Get all Edges
                edge = (line[i], line[i+2])
                if edge not in edges.keys():
                    edges[edge] = [user_idx]
                else:
                    edges[edge].append(user_idx)

    # Generate Graphviz Script
    head(args)
    for nidx, miner in nodes.items():
        gen_node(args, nidx, miner, isNormal=miner in normal)
    for item, label in edges.items():
        gen_edge(args, nodes, item, label)
    tail(args)

    # Store Selfish Mining Attack Times
    win = sum(win)
    with open(args.selfish, 'a') as fout:
        out = "{},{}\n".format(args.timestamp, win)
        fout.write(out)

    if args.display:
        os.system("dot -Tpng {} > {}".format(args.output, args.graph))
        os.system("open -a 'Visual Studio Code.app' {}".format(args.graph))