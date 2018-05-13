;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((c++-mode
  (flycheck-checker . c/c++-googlelint)
  (c-file-style . "Google")
  (c-tab-always-indent . t))
 (c-mode
  (mode . c++)
  (flycheck-checker . c/c++-googlelint)
  (c-file-style . "Google")
  (c-tab-always-indent . t)))
