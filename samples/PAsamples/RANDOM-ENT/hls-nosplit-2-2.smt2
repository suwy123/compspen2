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

(declare-const x0 Int)
(declare-const x1 Int)
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const x4 Int)
(declare-const x5 Int)
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const x8 Int)

(declare-const z0 Int)
(declare-const z1 Int)
(declare-const z2 Int)
(declare-const v0 Int)
(declare-const v1 Int)
(declare-const v2 Int)

(assert
	(and
	(> u0 2)
	(= v0 u0)
	(= (+ t0 u0) x2)
	(= x0 (+ t0 1))
	(< x2 x1)
	(< x0 x2)

	(> u1 2)
	(= v1 u1)
	(= (+ t1 u1) x5)
	(= x3 (+ t1 1))
	(< x5 x4)
	(< x3 x5)

	(> u2 2)
	(= v2 u2)
	(= (+ t2 u2) x8)
	(= x6 (+ t2 1))
	(< x8 x7)
	(< x6 x8)
	(sep
		(pto t0 (c_data_t u0))
		(blk x0 x1)
		(pto t1 (c_data_t u1))
		(blk x3 x4)
		(pto t2 (c_data_t u2))
		(blk x6 x7)
)))

(assert(not
	(sep
		(hls t0 x2 v0)
		(blk x2 x1)
		(hls t1 x5 v1)
		(blk x5 x4)
		(hls t2 x8 v2)
		(blk x8 x7)
	)
))

(check-sat)
;;unsat 43.62938
