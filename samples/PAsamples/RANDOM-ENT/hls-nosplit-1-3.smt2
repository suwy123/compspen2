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

(declare-const t0 Int)
(declare-const u0 Int)
(declare-const t1 Int)
(declare-const u1 Int)
(declare-const t2 Int)
(declare-const u2 Int)
(declare-const t3 Int)
(declare-const u3 Int)

(declare-const x0 Int)
(declare-const x1 Int)
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const x4 Int)
(declare-const x5 Int)
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const x8 Int)
(declare-const x9 Int)
(declare-const x10 Int)
(declare-const x11 Int)
(declare-const x12 Int)
(declare-const x13 Int)

(declare-const v0 Int)
(declare-const v1 Int)
(declare-const v2 Int)
(declare-const v3 Int)

(assert
	(and
	(> u0 2)
	(= v0 u0)
	(= (+ t0 u0) x1)
	(= x0 (+ t0 1))

	(> u1 2)
	(= v1 u1)
	(= (+ t1 u1) x5)
	(= x4 (+ t1 1))
	
	(> u2 2)
	(= v2 u2)
	(= (+ t2 u2) x9)
	(= x8 (+ t2 1))

	(> u3 2)
	(= v3 u3)
	(= (+ t3 u3) x13)
	(= x12 (+ t3 1))
	(sep
		(pto t0 (c_data_t u0))
		(blk x0 x1)
		(blk x2 x3)
		(pto t1 (c_data_t u1))
		(blk x4 x5)
		(blk x6 x7)
		(pto t2 (c_data_t u2))
		(blk x8 x9)
		(blk x10 x11)
		(pto t3 (c_data_t u3))
		(blk x12 x13)
)))

(assert(not
	(sep
		(hls t0 x1 v0)
		(blk x2 x3)
		(hls t1 x5 v1)
		(blk x6 x7)
		(hls t2 x9 v2)
		(blk x10 x11)
		(hls t3 x13 v3)
	)
))

(check-sat)
;;unsat 31.207604
