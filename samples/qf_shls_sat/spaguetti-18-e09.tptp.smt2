(set-logic QF_SLID_INT)

(set-info :source |
A. Rybalchenko and J. A. Navarro Perez.
[Separation Logic + Superposition Calculus = Heap Theorem Prover]
[PLDI 2011]
http://navarroj.com/research/papers.html#pldi11
|)
(set-info :smt-lib-version 2.0)
(set-info :category "random") 
(set-info :status unsat)
(set-info :version "2014-05-28")

; Sorts for locations, one by cell sort
(declare-sort RefSll_t 0)

; Types of cells in the heap

(declare-datatypes (
	(Sll_t 0)
	) (
	((c_Sll_t (next RefSll_t) ))
	)
)

; Type of heap

(declare-heap (RefSll_t Sll_t) 
)

(define-fun-rec ls ((in RefSll_t)(out RefSll_t)) Bool
	(or 
		(and 
			(= in out)
			(_ emp RefSll_t Sll_t)
		)

		(exists ((u RefSll_t))
	 
		(and 
			;(distinct in out)
		(sep 
			(pto in (c_Sll_t u ))
			(ls u out )
		)

		)

		)

	)
)


 
;; variables
(declare-const x0 RefSll_t)
(declare-const x1 RefSll_t)
(declare-const x2 RefSll_t)
(declare-const x3 RefSll_t)
(declare-const x4 RefSll_t)
(declare-const x5 RefSll_t)
(declare-const x6 RefSll_t)
(declare-const x7 RefSll_t)
(declare-const x8 RefSll_t)
(declare-const x9 RefSll_t)
(declare-const x10 RefSll_t)
(declare-const x11 RefSll_t)
(declare-const x12 RefSll_t)
(declare-const x13 RefSll_t)
(declare-const x14 RefSll_t)
(declare-const x15 RefSll_t)
(declare-const x16 RefSll_t)
(declare-const x17 RefSll_t)
(declare-const x18 RefSll_t)
(declare-const x19 RefSll_t)
(declare-const x20 RefSll_t)
(declare-const x21 RefSll_t)
(declare-const x22 RefSll_t)

(assert 
		(and 
			(= nil nil)
			(distinct x11 x12)
			(distinct x11 x14)
			(distinct x7 x8)
			(distinct x7 x18)
			(distinct x2 x6)
			(distinct x2 x8)
			(distinct x2 x15)
			(distinct x1 x8)
			(distinct x1 x3)
			(distinct x1 x9)
			(distinct x1 x16)
			(distinct x13 x14)
			(distinct x16 x17)
			(distinct x6 x8)
			(distinct x6 x10)
			(distinct x3 x12)
			(distinct x9 x16)
			(distinct x9 x13)
			(distinct x9 x17)
			(distinct x12 x17)
			(distinct x15 x17)
			(distinct x14 x15)
			(distinct x8 x14)
			(distinct x4 x7)
			(distinct x4 x16)
			(distinct x10 x11)
			(distinct x5 x18)
			(distinct x5 x10)
		(sep 
			(ls x13 x7 )
			(ls x13 x4 )
			(ls x13 x6 )
			(ls x10 x8 )
			(ls x18 x14 )
			(ls x18 x6 )
			(ls x8 x5 )
			(ls x17 x5 )
			(ls x17 x16 )
			(ls x2 x3 )
			(ls x9 x17 )
			(ls x7 x10 )
			(ls x3 x13 )
			(ls x3 x10 )
			(ls x11 x15 )
		)

		)

)

(check-sat)
