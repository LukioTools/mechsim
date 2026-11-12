# Mechsim

Mechsim is a highly customizable, dynamic joint and linkage simulator. It supports multiple constraint types, allowing you to create and simulate various mechanical systems.

## UI
Simulator has graphical user interface for the simulation visualization. Mouse position is captured while mouse is over the window. You can zoom and move around using mouse.
<br> <br>
simulation also has terminal user interface where you can inspect joints, constraints and linkages. When navigating, the graphical interface must be active, not the terminal.

Right now you can't edit or create new joints/constraints/linkages.

## The simulator uses three configuration files:

- `joints.txt` – Defines all joints used in the simulation.
- `constraints.txt` – Defines the constraints applied to joints.
- `linkages.txt` – Defines visual connections between joints.

### 1. Joints (`joints.txt`)

This file defines all joints used in the simulation.

IMPORTANT: A static joint named `mouse_position` must always be present. It is used to track the current mouse position.

#### There are two types of joints: static and dynamic.

#### Static joints

Static joints have fixed initial coordinates but can be moved by constraints during simulation.

Syntax:
`s <x> <y> <grounding> <visibility> <name>`

Parameters:
- `x`, `y` – Initial coordinates of the joint.
- `grounding` – Determines whether the joint is grounded.
- `visibility` – Determines whether the joint is visible.
- `name` – Unique identifier of the joint.

Example: `s 200 200 1 1 name`

#### Dynamic joints

Dynamic joints are calculated from other joints using basic arithmetic operations. They are always grounded and cannot be moved directly by constraints.

Supported operators: `+`, `-`, `*`, `/`.

Syntax: `d <joint> <operator> <joint> <visibility> <name>`

Parameters:
- First `joint` – The first input joint.
- `operator` – The arithmetic operation to perform.
- Second `joint` – The second input joint.
- `visibility` – Determines whether the resulting joint is visible.
- `name` – Unique identifier of the joint.

Example: `d mouse_position - joint 0 mouse_direction_from_joint`


Calculation rules:
- Addition (`+`) and subtraction (`-`) are performed element-wise on the joint coordinates.
- Multiplication (`*`) and division (`/`) use the length of the second joint as a scalar.
- All joints referenced by a dynamic joint must be defined before it.
- Dynamic joints can use either static or other dynamic joints as inputs.

### 2. Constraints (`constraints.txt`)

This file defines the constraints applied to joints during simulation.

The following constraint types are supported:

#### Distance constraint

Maintains a fixed distance between two joints.

Syntax: `dis <joint> <joint> <distance> <name>`

Parameters:
- First `joint` – First joint.
- Second `joint` – Second joint.
- `distance` – A joint whose length determines the required distance.
- `name` – Constraint identifier.

#### Direction constraint

Constrains the direction between two joints.

Syntax: `dir <joint> <joint> <direction> <name>`

Parameters:
- First `joint` – First joint.
- Second `joint` – Second joint.
- `direction` – Defines the required direction.
- `name` – Constraint identifier.

#### Linear constraint

Restricts a joint to move along a specified line.

Syntax: `lin <joint> <pos> <direction> <name>`

Parameters:
- `joint` – Joint being constrained.
- `pos` – A point defining the line's position.
- `direction` – Defines the line's direction.
- `name` – Constraint identifier.

#### Limit constraint

Restricts a joint's movement to a rectangular area defined by minimum and maximum coordinates.

Syntax: `lim <joint> <min_limit> <max_limit> <name>`

Parameters:
- `joint` – Joint being constrained.
- `min_limit` – Minimum corner.
- `max_limit` – Maximum corner.
- `name` – Constraint identifier.

NOTE: Constraint names are currently not used by the simulator, but they are mandatory and must be provided.

Adding new constraint types is relatively straightforward.

### 3. Linkages (`linkages.txt`)

This file defines visual connections between joints.

Linkages are purely visual and do not affect the simulation or apply any constraints. They are useful for visualizing mechanical structures and relationships between joints.

Syntax: `<joint> <joint> <name>`

Parameters:
- First `joint` – First joint.
- Second `joint` – Second joint.
- `name` – Linkage identifier.

Example:
joint_a joint_b linkage_name

Linkage names are currently not used by the simulator, but they are mandatory.

## 4. Troubleshooting

If Mechsim crashes during startup, check the following:

1. Missing joints: Make sure every joint referenced by a constraint or dynamic joint has been defined.
2. Definition order: All input joints used by a dynamic joint must be defined before that dynamic joint.
3. Mouse position: Make sure a joint named `mouse_position` is defined.
4. Invalid operators: Dynamic joints only support `+`, `-`, `*`, and `/`.
5. Missing names: Every joint, constraint, and linkage must have a name, even if the simulator does not use it.
6. If it can't solve constraints, you can disable position resetting Solver.hpp after failed try to debug. You may want to rewrite constraints and joints in a different way as two different configuration (which should result to same outcome) may have different "solvability" as sensitivy of the system may change.

GENERAL RECOMMENDATION: If the simulator crashes during launch, start by checking `joints.txt` and `constraints.txt`. Incorrect joint references and definition order are common causes of startup problems.

## License

Mechsim is licensed under the [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.html).

You are free to use, modify, and redistribute this software under the terms of the GPL-3.0 license.

See the [LICENSE](LICENSE) file for the full license text