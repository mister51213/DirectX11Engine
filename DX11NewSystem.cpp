System Tick

	Timer Tick

	Update Input

	World Tick
		Physics Tick
		Actor Positions
		Camera Position

	Graphics Update Frame (pWorld, pUI)
		Update World
		Update UI

	Graphics Draw Frame
		Draw World
			Update Camera Viewpoint
			Load Actor Positions
			Load Light Positions
			Load Actor Models
			Load Actor Materials
				Draw Material Texture (list or single?)
				Draw Material Shader
		Draw UI
			FPS string
			Position string
			Rotation string
			Render count string

	UI Tick
