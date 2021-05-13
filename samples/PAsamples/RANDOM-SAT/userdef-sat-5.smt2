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

;; variables
(declare-const x0 Int)
(declare-const sz0 Int)
(declare-const x1 Int)

(declare-const y0 Int)
(declare-const y1 Int)

(declare-const z0 Int)
(declare-const z1 Int)

(assert
	(and
	(< y1 z0)
	(sep
		(pto x0 (c_data_t sz0))
		(blk (+ x0 1) x1)
		(hck y0 y1)
		(hls z0 z1)
	)
	)	)
;; end of problem
(check-sat)
;; 0.03s sat
