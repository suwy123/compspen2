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
(define-fun hck ((?x Int) (?y Int)) Bool
	(and
		(>= (- ?y ?x) 2)
		(sep
			(pto ?x (c_data_t (- ?y ?x)))
			(blk (+ ?x 1) ?y)
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
			)
		)
	)
)

;; variables
(declare-const hst Int)
(declare-const hen Int)
(declare-const x0 Int)
(declare-const y0 Int)
(declare-const x1 Int)
(declare-const y1 Int)
(declare-const z0_0 Int)
(declare-const sz0_0 Int)
(declare-const u0_0 Int)
(declare-const v0_0 Int)
(declare-const z0_1 Int)
(declare-const sz0_1 Int)
(declare-const u0_1 Int)
(declare-const v0_1 Int)

(assert
	(and
	(= x0 hst)
	(= y1 hen)
	(< x0 y1)
	(= y0 z0_0)
	(= (+ z0_0 sz0_0) x1)
	(= (+ z0_0 1) z0_1)
	(sep
		(hls x0 y0)
		(pto z0_0 (c_data_t sz0_0))
		(pto z0_1 (c_data_t sz0_1))
		(hls x1 y1)
	)
))
(assert (not
	(sep
		(hls x0 y1)
		(hls y1 y1)
	)
))
;; end of problem
(check-sat)
;; 0.05s sat : the entl is false
