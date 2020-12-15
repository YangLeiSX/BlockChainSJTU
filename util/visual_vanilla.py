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

def gen_node(args, nidx, miner):
    """Write Node Lines for Graphviz Script

    Args:
        args (argparse.ArgumentParser): Command Line Parameters
        nidx (str): Block ID
        miner (str): Miner of Block
    """
    out = "\t\"#{}\\nminer#{}\"\n".format(nidx, miner)
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
                        default="visual/dot_vanilla.dot",
                        help="存放脚本的位置")
    parser.add_argument("-g","--graph", 
                        type=str,
                        default="visual/png_vanilla.png",
                        help="存放图片的位置")
    parser.add_argument("-v","--vanilla", 
                        type=str,
                        default="res_vanilla.csv",
                        help="存放普通测试结果的位置")
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
    chain_len = []
    seen = []

    with open(args.result, 'r') as fin:
        for line in fin.readlines():
            # remove '\n'
            line = line.split(',')[:-1]
            user_idx = line[0]
            seen.append(int(line[1]))
            line = line[2:]
            chain_len.append(len(line) // 2)
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
        gen_node(args, nidx, miner)
    for item, label in edges.items():
        gen_edge(args, nodes, item, label)
    tail(args)
    
    # Store Length Information and Number of Blocks
    seen = list(set(seen))
    chain_len = list(set(chain_len))
    if len(chain_len) <= 3:
        with open(args.vanilla, 'a') as fout:
            out = "{},{},{}\n".format(
                args.timestamp, repr(seen[0]), sum(chain_len) / len(chain_len)
            )
            fout.write(out)

    if args.display:
        os.system("dot -Tpng {} > {}".format(args.output, args.graph))
        os.system("open -a 'Visual Studio Code.app' {}".format(args.graph))