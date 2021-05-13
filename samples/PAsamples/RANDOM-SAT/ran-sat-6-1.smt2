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

(declare-const z0 Int)
(declare-const z1 Int)
(declare-const z2 Int)
(declare-const z3 Int)
(declare-const z4 Int)
(declare-const z5 Int)
(declare-const z6 Int)
(declare-const z7 Int)
(declare-const z8 Int)
(declare-const z9 Int)
(declare-const z10 Int)
(declare-const z11 Int)

(declare-const w0 Int)
(declare-const w1 Int)
(declare-const w2 Int)
(declare-const w3 Int)
(declare-const w4 Int)
(declare-const w5 Int)
(declare-const w6 Int)
(declare-const w7 Int)
(declare-const w8 Int)
(declare-const w9 Int)
(declare-const w10 Int)
(declare-const w11 Int)

(declare-const v0 Int)
(declare-const v1 Int)
(declare-const v2 Int)
(declare-const v3 Int)
(declare-const v4 Int)
(declare-const v5 Int)
(declare-const v6 Int)
(declare-const v7 Int)
(declare-const v8 Int)
(declare-const v9 Int)

(assert
	(and
	(= x0 1)
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
		(hck z0 z1 v0)
		(hck z1 z2 v1)
		(hck z2 z3 v2)
		(hck z3 z4 v3)
		(hck z4 z5 v4)
		(hck z6 z7 v5)
		(hck z7 z8 v6)
		(hck z8 z9 v7)
		(hck z9 z10 v8)
		(hck z10 z11 v9)
		(hls w0 w1 v0)
		(hls w1 w2 v1)
		(hls w2 w3 v2)
		(hls w3 w4 v3)
		(hls w4 w5 v4)
		(hls w6 w7 v5)
		(hls w7 w8 v6)
		(hls w8 w9 v7)
		(hls w9 w10 v8)
		(hls w10 w11 v9)
	)
))

(check-sat)
;;sat 0.529687