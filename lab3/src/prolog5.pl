my_length([], 0).
my_length([_|L], N):-my_length(L, M), N is M + 1.


pr :- pr.
pr.

my_remove(X, [X|T], T).
my_remove(X, [H|T], [H|R]):-my_remove(X, T, R).

pa(R, [X | T]) :- pa(R1, T), R is R1 + X*X.
pa(1, []).


p(R, R, []).
p(R, R1, [L|T]) :- N is R1 + L*L, p(R, N, T).
p(R, X) :- p(R, 1, X).

del_last(List, Res) :- append(Res, [_], List).

dell([]) :- write(1), !.
dell([X | T]) :- X is X + 1, write(X), dell(T).


summ(L, R) :- summ(L, 1, [], R).
summ(L, N, K, R) := append([X], T, L), Y is X + N, N1 is N + 1, append(K, [Y], Z), summ(T, N1, Z, R).

summ([], Z, R, R]).