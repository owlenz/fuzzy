proj_funding IN [0, 100]
exp_level IN [0, 60]
risk OUT [0, 100]

exp_level
beginner TRI 0 15 30
intermediate TRI 15 30 45
expert TRI 30 60 60

proj_funding
very_low TRAP 0 0 10 30
low TRAP 10 30 40 60
medium TRAP 40 60 70 90
high TRAP 70 90 100 100

risk
low TRI 0 25 50
normal TRI 25 50 75
high TRI 50 100 100

proj_funding high or exp_level expert => risk low
proj_funding medium and exp_level intermediate => risk normal
proj_funding medium and exp_level beginner => risk normal
proj_funding low and exp_level beginner => risk high
proj_funding very_low and_not exp_level expert => risk high

proj_funding: 50
exp_level: 40
