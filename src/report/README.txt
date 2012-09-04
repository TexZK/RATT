Final report for the Embedded systems / RTOS courses, held at the Politecnico
di Milano by William Fornaciari and Patrick Bellasi.
http://home.dei.polimi.it/fornacia/
http://home.dei.polimi.it/bellasi/

Report written by Andrea Zoppi, mat.765662 (aka TexZK).
andrea.zoppi@mail.polimi.it - texzk@email.it (recommended)

PDF generated with LaTeX + MiKTeX



*ABSTRACT*

Cheap jogwheel encoders for emulated DJ turntables are often inaccurate, due
to the low CPR in the order of some tens. Precise encoders are rather expensive
and are not convenient to keep track of very fast rotations, because they
are too much accurate for the purpose.

The presented research tries to improve the detection of at least small and
slow relative rotations of the jogwheel, by employing a cheap COTS mouse sensor,
and keep the absolute position or fast rotation with the classic cheap optical
encoder.

A very crude HID demoboard was developed, so that some simple tests were done.
An extended proposal is also described, in order to achieve better performance
and more features, to match those of a commercial DJ controller.

