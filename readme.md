# Problem Statement
Let R be a relation with a set A of attributes and let r be an instance of R. 
A functional dependency (FD) is an expression of the form X->Y, where
X is subset of A and Y is \in A. The dependency is valid in the instance r if and
only if for every pair of rows (tuples) t, u which are in r, whenever t[B]=u[B] for all
B in X, it is also the case that t[Y]=u[Y].

Our problem can be formally stated as follows: given a relation
R and an instance r of R, find all non-trivial and minimal FDs that are valid in r. 


## Method proposed
We prove that this problem can be reduced to the problem to find refutations and then compute minimal transversals to its complement.
