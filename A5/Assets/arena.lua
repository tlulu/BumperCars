-- arena.lua

rootnode = gr.node('root')
-- rootnode:rotate('y', -20.0)
-- rootnode:scale( 0.25, 0.25, 0.25 )
-- rootnode:translate(0.0, -0.25, -1.5)

pink = gr.material({1.0, 0.752941, 0.796078}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.596078, 0.984314, 0.596078}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
mint = gr.material({1.0, 0.937255, 0.835294}, {0.1, 0.1, 0.1}, 10)
turquoise = gr.material({0.282353, 0.819608, 0.8}, {0.1, 0.1, 0.1}, 10)

ground = gr.mesh('cube', 'ground')
rootnode:add_child(ground)
ground:set_material(white)
ground:scale(2.8, 1.0, 1.4)

-- GOALS

goal1 = gr.mesh('cube', 'goal1')
rootnode:add_child(goal1)
goal1:set_material(pink)
goal1:scale(0.2, 1.0, 0.4)
goal1:translate(-1.5, 0.0, 0.0)

goal2 = gr.mesh('cube', 'goal2')
rootnode:add_child(goal2)
goal2:set_material(green)
goal2:scale(0.2, 1.0, 0.4)
goal2:translate(1.5, 0.0, 0.0)

-- NEAR/FAR

wall_far = gr.mesh('cube', 'wall_far')
rootnode:add_child(wall_far)
wall_far:set_material(mint)
wall_far:scale(2.8, 0.1, 0.02)
wall_far:translate(0.0, 0.55, -0.69)

wall_near = gr.mesh('cube', 'wall_near')
rootnode:add_child(wall_near)
wall_near:set_material(mint)
wall_near:scale(2.8, 0.1, 0.02)
wall_near:translate(0.0, 0.55, 0.69)

-- RIGHT

wall_right = gr.mesh('cube', 'wall_right')
rootnode:add_child(wall_right)
wall_right:set_material(mint)
wall_right:scale(0.02, 0.1, 0.5)
wall_right:translate(1.39, 0.55, -0.45)

wall_right2 = gr.mesh('cube', 'wall_right2')
rootnode:add_child(wall_right2)
wall_right2:set_material(mint)
wall_right2:scale(0.02, 0.1, 0.5)
wall_right2:translate(1.39, 0.55, 0.45)

goal_right = gr.mesh('cube', 'goal_right')
rootnode:add_child(goal_right)
goal_right:set_material(mint)
goal_right:scale(0.2, 0.1, 0.02)
goal_right:translate(1.4 + 0.1, 0.55, -0.21)

goal_right2 = gr.mesh('cube', 'goal_right2')
rootnode:add_child(goal_right2)
goal_right2:set_material(mint)
goal_right2:scale(0.02, 0.1, 0.44)
goal_right2:translate(1.4 + 0.2, 0.55, 0.0)

goal_right3 = gr.mesh('cube', 'goal_right3')
rootnode:add_child(goal_right3)
goal_right3:set_material(mint)
goal_right3:scale(0.2, 0.1, 0.02)
goal_right3:translate(1.4 + 0.1, 0.55, 0.21)

-- LEFT

wall_left = gr.mesh('cube', 'wall_left')
rootnode:add_child(wall_left)
wall_left:set_material(mint)
wall_left:scale(0.02, 0.1, 0.5)
wall_left:translate(-1.39, 0.55, -0.45)

wall_left2 = gr.mesh('cube', 'wall_left2')
rootnode:add_child(wall_left2)
wall_left2:set_material(mint)
wall_left2:scale(0.02, 0.1, 0.5)
wall_left2:translate(-1.39, 0.55, 0.45)

goal_left = gr.mesh('cube', 'goal_left')
rootnode:add_child(goal_left)
goal_left:set_material(mint)
goal_left:scale(0.2, 0.1, 0.02)
goal_left:translate(-(1.4 + 0.1), 0.55, -0.21)

goal_left2 = gr.mesh('cube', 'goal_left2')
rootnode:add_child(goal_left2)
goal_left2:set_material(mint)
goal_left2:scale(0.02, 0.1, 0.44)
goal_left2:translate(-(1.4 + 0.2), 0.55, 0.0)

goal_left3 = gr.mesh('cube', 'goal_left3')
rootnode:add_child(goal_left3)
goal_left3:set_material(mint)
goal_left3:scale(0.2, 0.1, 0.02)
goal_left3:translate(-(1.4 + 0.1), 0.55, 0.21)

return rootnode
