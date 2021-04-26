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
(declare-const x2 Int)
(declare-const y2 Int)
(declare-const z0_0 Int)
(declare-const sz0_0 Int)
(declare-const u0_0 Int)
(declare-const v0_0 Int)
(declare-const z1_0 Int)
(declare-const sz1_0 Int)
(declare-const u1_0 Int)
(declare-const v1_0 Int)

(assert
	(and
	(= x0 hst)
	(= y2 hen)
	(< x0 y2)
	(= y0 z0_0)
	(= y1 z1_0)
	(= v1_0 x2)
	(= (+ z0_0 sz0_0) x1)
	(= (+ z1_0 sz1_0) x2)
	(= (+ z1_0 1) u1_0)
	(= v1_0 x2)
	(sep
		(hls x0 y0)
		(pto z0_0 (c_data_t sz0_0))
		(hls x1 y1)
		(pto z1_0 (c_data_t sz1_0))
		(hls x2 y2)
	)
))
(assert (not
	(sep
		(hls x0 y0)
		(hls y0 y2)
	)
))
;; end of problem
(check-sat)
;; 0.06s sat : the entl is false
