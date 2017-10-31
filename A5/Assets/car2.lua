-- car.lua

rootnode = gr.node('root')
--rootnode:rotate('y', -20.0)
rootnode:scale( 0.25, 0.25, 0.25 )
-- rootnode:translate(0.0, -0.25, -1.5)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

car = gr.mesh('sphere', 'car')
rootnode:add_child(car)
car:set_material(green)
car:scale(0.45, 0.1, 0.4)

car_body = gr.mesh('sphere', 'body')
car:add_child(car_body)
car_body:set_material(white)
car_body:scale(1.0/0.45, 1.0/0.1, 1.0/0.4)
car_body:scale(0.35, 0.15, 0.3)
car_body:translate(0.0, 1.0, 0.0)

torso = gr.mesh('cube', 'torso')
car_body:add_child(torso)
torso:set_material(blue)
torso:scale(1.0/0.35, 1.0/0.15, 1.0/0.3)
torso:scale(0.2, 0.2, 0.2)
torso:translate(0.1, 0.8, 0.0)

neck = gr.mesh('sphere', 'neck')
torso:add_child(neck)
neck:scale(1.0/0.2, 1.0/0.2, 1.0/0.2)
neck:scale(0.05, 0.1, 0.05)
neck:translate(0.0, 0.7, 0.0)
neck:set_material(blue)

head = gr.mesh('cube', 'head')
torso:add_child(head)
head:scale(1.0/0.2, 1.0/0.2, 1.0/0.2)
head:scale(0.12, 0.12, 0.12)
head:translate(0.0, 1.0, 0.0)
head:set_material(green)

return rootnode
