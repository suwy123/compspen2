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

(declare-const x0 Int)
(declare-const x1 Int)
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const x4 Int)
(declare-const x5 Int)

(declare-const z0 Int)
(declare-const z1 Int)
(declare-const z2 Int)
(declare-const v0 Int)
(declare-const v1 Int)

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
	(sep
		(pto t0 (c_data_t u0))
		(blk x0 x1)
		(pto t1 (c_data_t u1))
		(blk x3 x4)
)))

(assert(not
	(sep
		(hls t0 x2 v0)
		(blk x2 x1)
		(hls t1 x5 v1)
		(blk x5 x4)
	)
))

(check-sat)
;;unsat 8.736093
