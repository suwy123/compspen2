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
(declare-const rsz Int)
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
(declare-const xnxt Int)

(assert
	(and
	(distinct hst hen)
	(= hst x0)
	(= (+ x0 sz0) x1)
	(< sz0 rsz)
	(= (+ x1 sz1) x2)
	(< sz1 rsz)
	(= (+ x2 sz2) x3)
	(< sz2 rsz)
	(= (+ x3 sz3) x4)
	(< sz3 rsz)
	(= (+ x4 sz4) x5)
	(< sz4 rsz)
	(= (+ x5 sz5) x6)
	(< sz5 rsz)
	(= (+ x6 sz6) xnxt)
	(>= sz6 rsz)
	(sep
		(hck x0 x1)
		(hck x1 x2)
		(hck x2 x3)
		(hck x3 x4)
		(hck x4 x5)
		(hck x5 x6)
		(hck x6 xnxt)
		(hls xnxt hen)
	)
	)	)
;; end of problem
(check-sat)
;; 0.04s sat
