-- ball.lua

rootnode = gr.node('root')
-- rootnode:rotate('y', -20.0)
-- rootnode:scale( 0.25, 0.25, 0.25 )
-- rootnode:translate(0.0, -0.25, -1.5)

yellow = gr.material({1.0, 0.843137, 0.0}, {0.1, 0.1, 0.1}, 10)

ball = gr.mesh('sphere', 'ball')
rootnode:add_child(ball)
ball:set_material(yellow)
ball:scale(0.025, 0.025, 0.025)

return rootnode
