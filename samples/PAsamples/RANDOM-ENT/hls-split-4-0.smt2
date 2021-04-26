(set-logic QF_SLAH)

(declare-datatypes (
	(data_t 0)
	) (
	((c_data_t (sz Int)))
	)
)


(declare-heap (Int data_t) 
)

;; heap chunk
(define-fun hck ((?x Int) (?y Int) (?v Int)) Bool
	(and
		(>= (- ?y ?x) 2) (<= (- ?y ?x) ?v)
		(sep
			(pto ?x (c_data_t (- ?y ?x)))
			(blk (+ ?x 1) ?y)
		)
	)
)
;; heap list
(define-fun-rec hls ((?x Int) (?y Int) (?v Int)) Bool
	(or
		(and (= ?x ?y) (_ emp Int data_t))
		(exists ((?w Int))
			(sep
				(hck ?x ?w ?v)
				(hls ?w ?y ?v)
			)
		)
	)
)

;; variables
(declare-const x0 Int)
(declare-const x1 Int)
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const x4 Int)
(declare-const x5 Int)

(declare-const z0 Int)
(declare-const z1 Int)
(declare-const z2 Int)
(declare-const z3 Int)
(declare-const z4 Int)
(declare-const z5 Int)
(declare-const v Int)

(assert
	(and
	(= v 5)
	(= z0 x0)
	(< z0 x1)
	(< x1 z1)
	(< z0 x2)
	(< x2 z1)
	(< x1 x2)
	(< z1 x3)
	(< x3 z2)
	(< x3 x4)
	(< z1 x4)
	(< x4 z2)
	(= x5 z2)
	(sep
		(hls z0 z1 v)
		(hls z1 z2 v)
)))

(assert(not
	(sep
		(blk x0 x1)
		(blk x1 x2)
		(blk x2 x3)
		(blk x3 x4)
		(blk x4 x5)
	)
))

(check-sat)
;;unsat 13.807173
