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
(declare-const sz0 Int)
(declare-const x1 Int)
(declare-const sz1 Int)
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const sz2 Int)
(declare-const x4 Int)
(declare-const x5 Int)
(declare-const x6 Int)

(declare-const y0 Int)
(declare-const y1 Int)
(declare-const y2 Int)
(declare-const y3 Int)
(declare-const y4 Int)
(declare-const y5 Int)
(declare-const y6 Int)

(declare-const z0 Int)
(declare-const z1 Int)
(declare-const z2 Int)
(declare-const z3 Int)
(declare-const z4 Int)
(declare-const v0 Int)
(declare-const v1 Int)
(declare-const v2 Int)
(declare-const v3 Int)

(assert
	(and
	(= v0 5)
	(= v1 8)
	(<= v1 v2)
	(<= v2 v3)
	(<= (+ z1 3) z2)
	(<= (+ z3 3) z4)
	(= x1 z1)
	(= x2 z2)
	(< z4 x3)
	(= x5 x4)
	(< z4 y2)
	(< y3 x3)
	(= v2 (- y6 y5))
	(= sz1 (- z4 x1))
	(sep
		(pto x0 (c_data_t sz0))
		(blk (+ x0 1) x1)
		(hck y0 y1 v0)
		(hls z0 z1 v1)
		(pto x1 (c_data_t sz1))
		(blk (+ x1 1) x2)
		(hls z2 z3 v2)
		(hls z3 z4 v3)
		(hck y2 y3 v0)
		(pto y3 (c_data_t sz2))
		(blk x3 x4)
		(blk x5 x6)
		(pto x6 (c_data_t sz2))
		(hck y4 y5 v0)
		(hck y5 y6 v2)
)))

(check-sat)
;;sat 0.066244
