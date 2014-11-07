import itertools
import sys

def kbits(n, k):
    result = []
    counters = []
    counters = [1]*n
    
    aux = [1] * n
    result.append(','.join("{0}".format(x) for x in aux))
    for bits in itertools.combinations(range(n), k):
        s = [0] * n
        for bit in bits:
            s[bit] = 1
        for i in range(len(s)):
            if s[i]==0:
            	counters[i]+=1
            	s[i] = counters[i]
        result.append(','.join("{0}".format(y) for y in s))
    return result

result = kbits(int(sys.argv[1]), int(int(sys.argv[1])/2))
fo = open(sys.argv[2], "w+")
for i in range(len(result)):  
    fo.write(result[i]+"\n") 
    print result[i]
fo.close()
