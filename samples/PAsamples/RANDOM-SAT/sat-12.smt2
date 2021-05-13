(set-logic QF_SLAH)

(declare-datatypes (
	(data_t 0)
	) (
	((c_data_t (data Int)))
	)
)


(declare-heap (Int data_t) 
)

;; heap chunk
(define-fun hck ((?x Int) (?y Int)) Bool
	(exists ((?f Int))
	(and
		(or
			(and (>= (- ?y ?x) 3) (= ?f 0))
			(and (>= (- ?y ?x) 3) (= ?f 1))
		)
		(sep
				(pto ?x (c_data_t (- ?y ?x)))
				(pto (+ ?x 1) (c_data_t ?f))
				(blk (+ ?x 2) ?y)
		)
	)
	)
)
;; heap list
(define-fun-rec hls ((?x Int) (?y Int)) Bool
	(or
		(and (= ?x ?y) (_ emp Int data_t))
		(exists ((?w Int))
			(sep
					(hck ?x ?w)
					(hls ?w ?y)
		))
	)
)



;; declare variables
(declare-const t1 Int)
(declare-const u1 Int)
(declare-const x1 Int)
(declare-const y1 Int)
(declare-const y2 Int)
(declare-const z2 Int)


(assert
	(and
		(> y1 (- y2 3))
		(sep
			(pto t1 (c_data_t u1))
			(blk x1 y1)
			(hck y1 y2)
			(hls y2 z2)
		)
	)
)

(check-sat)
;;0.03s unsat
