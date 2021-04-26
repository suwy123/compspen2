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

(declare-const x0 Int)
(declare-const x1 Int)
(declare-const x2 Int)

(declare-const z0 Int)
(declare-const z1 Int)
(declare-const z2 Int)
(declare-const v Int)

(assert
	(and
	(> u0 2)
	(= v u0)
	(= (+ t0 u0) x1)
	(= x0 (+ t0 1))
	(sep
		(pto t0 (c_data_t u0))
		(blk x0 x1)
)))

(assert(not
	(hls t0 x1 v)
))

(check-sat)
;;unsat 0.434463
