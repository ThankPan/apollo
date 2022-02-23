#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" Process the apollo task map, e.g., find key paths.
    Author: Yuting Xie
    2022.2.23
"""

from enum import Enum
import graphlib

TASK_MAP_FILENAME = "um_dev/profiling/task_map/typical_task_map.txt"

# A node could be a component or a channel
class GraphNodeType(Enum):
    COMPONENT = 1
    CHANNEL = 2
    
class GraphNode:
    def __init__(self, type: GraphNodeType, name):
        self.type = type
        self.name = name
        self.nexts = []
        self.precedences = []
    
    def pointsTo(self, other):
        self.nexts.append(other.name)
        other.precedences.append(self.name)

class Graph:
    def __init__(self):
        self.nodes = dict()
        self.ts_result = []

    def __str__(self) -> str:
        res = ""
        for name, node in self.nodes.items():
            res += f"{name}: ["
            for name_ in node.nexts:
                res += f"{name_}, "
            res += "]\n"
        return res

    
    def addNode(self, type: GraphNodeType, name):
        if name not in self.nodes:
            self.nodes[name] = GraphNode(type=type, name=name)
    
    def addDirectedEdge(self, name_from, name_to):
        assert name_from in self.nodes
        assert name_to in self.nodes
        self.nodes[name_from].pointsTo(self.nodes[name_to])

    def buildMapFromText(self, filename):
        with open(filename, 'r') as f:
            case = 0
            channel_name = ""
            message_type = ""
            ratio = 0.0
            for line in f.readlines():
                if len(line.strip()) == 0:
                    case = 0
                    continue
                if case == 0:
                    channel_name = line.strip()
                    self.addNode(type=GraphNodeType.CHANNEL, name=channel_name)
                elif case == 1:
                    message_type = line.strip()
                elif case == 2:
                    ratio = float(line.strip())
                elif case == 3:
                    for reader in line.split():
                        self.addNode(type=GraphNodeType.COMPONENT, name=reader)
                        self.addDirectedEdge(channel_name, reader)
                elif case == 4:
                    for writer in line.split():
                        self.addNode(type=GraphNodeType.COMPONENT, name=writer)
                        self.addDirectedEdge(writer, channel_name)
                else:
                    pass
                case = (case + 1) % 5

    def topologicalSort(self):
        self.ts_result = []
        finished = {}
        for name in self.nodes:
            finished[name] = False

        def hasNoPrecedence(node):
            for name in node.precedences:
                if not finished[name]:
                    return False
            return True

        def isFinished():
            for f in finished.values():
                if not f:
                    return False
            return True
        
        while not isFinished():
            cur = None
            for name, node in self.nodes.items():
                if not finished[name] and hasNoPrecedence(node):
                    cur = name
                    break
            if cur == None:
                print(f"{finished}")
                print(f"{self.ts_result}")
                self.ts_result = []
                return False
            queue = [cur]
            finished[cur] = True
            while len(queue) > 0:
                cur = queue[0]
                queue = queue[1:]
                self.ts_result.append(cur)
                for next in self.nodes[cur].nexts:
                    if not finished[next] and hasNoPrecedence(self.nodes[next]):
                        queue.append(next)
                        finished[next] = True

        
        return True
        


if __name__ == "__main__":
    graph = Graph()
    graph.buildMapFromText(TASK_MAP_FILENAME)
    print(graph)
    # assert graph.topologicalSort()
    # print(graph.ts_result)
