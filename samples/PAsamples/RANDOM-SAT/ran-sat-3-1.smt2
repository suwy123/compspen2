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
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const x8 Int)
(declare-const x9 Int)

(declare-const sz0 Int)
(declare-const sz1 Int)
(declare-const sz2 Int)
(declare-const sz3 Int)
(declare-const sz4 Int)
(declare-const sz5 Int)
(declare-const sz6 Int)
(declare-const sz7 Int)
(declare-const sz8 Int)
(declare-const sz9 Int)

(declare-const y0 Int)
(declare-const y1 Int)
(declare-const y2 Int)
(declare-const y3 Int)
(declare-const y4 Int)
(declare-const y5 Int)
(declare-const y6 Int)
(declare-const y7 Int)
(declare-const y8 Int)
(declare-const y9 Int)
(declare-const y10 Int)
(declare-const y11 Int)

(assert
	(and
	(= sz0 5)
	(= sz1 sz2)
	(< sz2 sz3)
	(= (+ x4 1) x5)
	(> y0 x9)
	(= y5 y6)
	(sep
		(pto x0 (c_data_t sz0))
		(pto x1 (c_data_t sz1))
		(pto x2 (c_data_t sz2))
		(pto x3 (c_data_t sz3))
		(pto x4 (c_data_t sz4))
		(pto x5 (c_data_t sz5))
		(pto x6 (c_data_t sz6))
		(pto x7 (c_data_t sz7))
		(pto x8 (c_data_t sz8))
		(pto x9 (c_data_t sz9))
		(blk y0 y1)
		(blk y1 y2)
		(blk y2 y3)
		(blk y3 y4)
		(blk y4 y5)
		(blk y6 y7)
		(blk y7 y8)
		(blk y8 y9)
		(blk y9 y10)
		(blk y10 y11)
	)
))

(check-sat)
;;sat 0.088237
