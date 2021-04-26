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

;; hst = x0 &
;; 256 <= sz0 &
;; x1 < hen &
;; x1 = hst + sz0 &
;; 256 <= sz1 &
;; xnxt = x1 + sz1 
;; :
;; integer(hst,sz0+sz1) *
;; blk(hst+1,x1) *
;; integer(x1, sz1) *
;; blk(x1+1,xnxt) *
;; hls(xnxt, hen)
;; 
;; ==>
;; 
;; hck(hst,sz0+sz1,xnxt) *
;; hls(xnxt,hen)

;; declare variables
(declare-const hst Int)
(declare-const hen Int)

(declare-const x0 Int)
(declare-const sz0 Int)
(declare-const x1 Int)
(declare-const sz1 Int)
(declare-const xnxt Int)
(declare-const szall Int)

(assert (and
		(< 256 sz0)
		(= hst x0)
		(= x1 (+ x0 sz0))
		(= xnxt (+ x1 sz1))
		(= szall (+ sz0 sz1))
		(sep
			(pto x0 (c_data_t sz0))
			(blk (+ x0 1) x1)
			(pto x1 (c_data_t sz1))
			(blk (+ x1 1) xnxt)
			(hls xnxt hen)
		)
	)
)

(assert (not
		(sep
			(pto hst (c_data_t szall))
			(blk (+ hst 1) xnxt)
			(hls xnxt hen)
		)
	)
)

(check-sat)
;; 0.75s sat : entl is false because sz0 != szall
