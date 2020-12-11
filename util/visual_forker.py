import os
import argparse

def head(args):
    with open(args.output, 'w') as fout:
        fout.write("digraph tree {\n")
        fout.write("\trankdir = LR\n")

def tail(args):
    with open(args.output, 'a') as fout:
        fout.write("}\n")

def gen_node(args, nidx, miner, isForker=False):
    out = "\t\"#{}\\nminer#{}\" [ color = {} ]\n".format(
        nidx, miner, "red" if isForker else "black")
    with open(args.output, 'a') as fout:
        fout.write(out)

def gen_edge(args, nodes, edge, label):
    out = "\t\"#{}\\nminer#{}\" -> \"#{}\\nminer#{}\"\n".format(
        edge[0], nodes[edge[0]], edge[1], nodes[edge[1]])
    with open(args.output, 'a') as fout:
        fout.write(out)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-r","--result", 
                        type=str,
                        default="result.csv",
                        help="存放结果的位置")
    parser.add_argument("-o","--output", 
                        type=str,
                        default="visual/dot_forker.dot",
                        help="存放脚本的位置")
    parser.add_argument("-g","--graph", 
                        type=str,
                        default="visual/png_forker.png",
                        help="存放图片的位置")
    parser.add_argument("-f","--forker", 
                        type=str,
                        default="res_forker.csv",
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

    nodes = {}
    edges = {}
    forkers = []
    forklen = []
    chainLen = []
    with open(args.result, 'r') as fin:
        for line in fin.readlines():
            # remove '\n'
            line = line.split(',')[:-1]
            user_idx = line[0]
            client_type = line[1]
            if client_type == 'F':
                forkers.append(user_idx)
            elif client_type not in forklen:
                forklen.append(client_type)
            line = line[2:]
            chainLen.append(len(line) // 2)
            for i in range(0, len(line)-1, 2):
                if line[i] not in nodes.keys():
                    nodes[line[i]] = line[i+1]
                if i == len(line)-2: 
                    continue
                edge = (line[i], line[i+2])
                if edge not in edges.keys():
                    edges[edge] = [user_idx]
                else:
                    edges[edge].append(user_idx)

    head(args)
    for nidx, miner in nodes.items():
        gen_node(args, nidx, miner, isForker=miner in forkers)
    for item, label in edges.items():
        gen_edge(args, nodes, item, label)
    tail(args)

    chainLen = list(set(chainLen))
    chainLen = sum(chainLen) / len(chainLen)

    forklen = forklen[0].split("|")[:-1]
    len_list = [int(l) for l in forklen]
    len_set = set(len_list)
    len_dict = {}
    for item in len_set:
        if item not in len_dict.keys():
            len_dict.update({item:len_list.count(item)})
    out_list = []
    for i in range(1, 6):
        if i in len_set:
            out_list.append(str(len_dict[i]))
        else:
            out_list.append(str(0))    
    out_line = ",".join(out_list)
    with open(args.forker, "a") as fout:
        out = "{},{:.1f},{}\n".format(args.timestamp, chainLen, out_line)
        fout.write(out)

    if args.display:
        os.system("dot -Tpng {} > {}".format(args.output, args.graph))
        os.system("open -a 'Visual Studio Code.app' {}".format(args.graph))