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
		(>= (- ?y ?x) 3)
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
(declare-const sz0 Int)
(declare-const x1 Int)
(declare-const sz1 Int)
(declare-const x2 Int)
(declare-const sz2 Int)
(declare-const x3 Int)
(declare-const sz3 Int)
(declare-const x4 Int)
(declare-const sz4 Int)
(declare-const x5 Int)
(declare-const sz5 Int)
(declare-const x6 Int)
(declare-const sz6 Int)
(declare-const x7 Int)
(declare-const sz7 Int)
(declare-const x8 Int)
(declare-const sz8 Int)
(declare-const xnxt Int)
(declare-const szall Int)

(assert
	(and

	(= szall (+ sz0 sz1 sz2 sz3 sz4 sz5 sz6 sz7 sz8))
	(= hst x0)
	(= (+ x0 sz0) x1)
	(= (+ x1 sz1) x2)
	(= (+ x2 sz2) x3)
	(= (+ x3 sz3) x4)
	(= (+ x4 sz4) x5)
	(= (+ x5 sz5) x6)
	(= (+ x6 sz6) x7)
	(= (+ x7 sz7) x8)
	(= (+ x8 sz8) xnxt)
	(sep
		(pto x0 (c_data_t szall))
		(blk (+ x0 1) x1)
		(pto x1 (c_data_t sz1))
		(blk (+ x1 1) x2)
		(pto x2 (c_data_t sz2))
		(blk (+ x2 1) x3)
		(pto x3 (c_data_t sz3))
		(blk (+ x3 1) x4)
		(pto x4 (c_data_t sz4))
		(blk (+ x4 1) x5)
		(pto x5 (c_data_t sz5))
		(blk (+ x5 1) x6)
		(pto x6 (c_data_t sz6))
		(blk (+ x6 1) x7)
		(pto x7 (c_data_t sz7))
		(blk (+ x7 1) x8)
		(pto x8 (c_data_t sz8))
		(blk (+ x8 1) xnxt)
		(hls xnxt hen)
	)
	)	)
;; end of problem
(check-sat)
;; 0.10s sat
