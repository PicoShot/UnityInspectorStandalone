---@meta
-- UnityInspector Lua API Stubs
-- Add this file to your VSCode workspace for IntelliSense/autocomplete
-- This is a stub file - it doesn't execute, it just provides type hints

-- ============================================
-- Math Types
-- ============================================

---@class Vector2
---@field x number
---@field y number
---@field Distance fun(self: Vector2, other: Vector2): number
---@overload fun(): Vector2
---@overload fun(x: number, y: number): Vector2
Vector2 = {}

---@class Vector3
---@field x number
---@field y number
---@field z number
---@field Length fun(self: Vector3): number
---@field Dot fun(self: Vector3, other: Vector3): number
---@field Normalize fun(self: Vector3): Vector3
---@field Distance fun(self: Vector3, other: Vector3): number
---@overload fun(): Vector3
---@overload fun(x: number, y: number, z: number): Vector3
Vector3 = {}

---@class Vector4
---@field x number
---@field y number
---@field z number
---@field w number
---@overload fun(): Vector4
---@overload fun(x: number, y: number, z: number, w: number): Vector4
Vector4 = {}

---@class Quaternion
---@field x number
---@field y number
---@field z number
---@field w number
---@field Euler fun(x: number, y: number, z: number): Quaternion
---@field ToEuler fun(self: Quaternion): Vector3
---@field LookRotation fun(forward: Vector3): Quaternion
---@overload fun(): Quaternion
---@overload fun(x: number, y: number, z: number, w: number): Quaternion
Quaternion = {}

---@class Color
---@field r number
---@field g number
---@field b number
---@field a number
---@overload fun(): Color
---@overload fun(r: number, g: number, b: number): Color
---@overload fun(r: number, g: number, b: number, a: number): Color
Color = {}

---@class Ray
---@field origin Vector3
---@field direction Vector3
Ray = {}

---@class Matrix4x4
Matrix4x4 = {}
---@param row number
---@param col number
---@return number
function Matrix4x4:Get(row, col) end

---@class Bounds
---@field center Vector3
---@field extents Vector3
Bounds = {}

---@class Plane
---@field normal Vector3
---@field distance number
Plane = {}

---@class Rect
---@field x number
---@field y number
---@field width number
---@field height number
---@overload fun(): Rect
---@overload fun(x: number, y: number, w: number, h: number): Rect
Rect = {}

---@class RaycastHit
---@field point Vector3
---@field normal Vector3
---@field distance number
---@field collider userdata
RaycastHit = {}

-- ============================================
-- Factory functions
-- ============================================

---@param x number
---@param y number
---@return Vector2
function MakeVector2(x, y) end

---@param x number
---@param y number
---@param z number
---@return Vector3
function MakeVector3(x, y, z) end

---@param x number
---@param y number
---@param z number
---@param w number
---@return Vector4
function MakeVector4(x, y, z, w) end

---@param x number
---@param y number
---@param z number
---@param w number
---@return Quaternion
function MakeQuaternion(x, y, z, w) end

---@param r number
---@param g number
---@param b number
---@param a? number
---@return Color
function MakeColor(r, g, b, a) end

---@param origin Vector3
---@param dir Vector3
---@return Ray
function MakeRay(origin, dir) end

---@return Matrix4x4
function MakeMatrix4x4() end

---@param center Vector3
---@param extents Vector3
---@return Bounds
function MakeBounds(center, extents) end

---@param normal Vector3
---@param distance number
---@return Plane
function MakePlane(normal, distance) end

---@param x number
---@param y number
---@param w number
---@param h number
---@return Rect
function MakeRect(x, y, w, h) end

-- ============================================
-- Base Unity Types
-- ============================================

---@class Object
---@field GetType fun(self: Object): userdata
---@field ToString fun(self: Object): string
---@field GetHashCode fun(self: Object): number
Object = {}

---@class UnityObject : Object
---@field IsAlive fun(self: UnityObject): boolean
---@field GetName fun(self: UnityObject): string
---@field Destroy fun(self: UnityObject)
---@field DontDestroyOnLoad fun(self: UnityObject)
UnityObject = {}

---@class String : Object
---@field ToString fun(self: String): string
String = {}

---@class Component : UnityObject
---@field GetTransform fun(self: Component): Transform
---@field GetGameObject fun(self: Component): GameObject
---@field GetTag fun(self: Component): string
---@field GetComponentInChildren fun(self: Component, type: Class): userdata
---@field GetComponentInParent fun(self: Component, type: Class): userdata
---@field GetComponents fun(self: Component, type: Class): table<number, userdata>
---@field GetComponentsInChildren fun(self: Component, type: Class): table<number, userdata>
---@field GetComponentsInParent fun(self: Component, type: Class): table<number, userdata>
Component = {}

---@class Behaviour : Component
---@field GetEnabled fun(self: Behaviour): boolean
---@field SetEnabled fun(self: Behaviour, value: boolean)
Behaviour = {}

---@class GameObject : UnityObject
---@field GetName fun(self: GameObject): string
---@field SetActive fun(self: GameObject, active: boolean)
---@field GetActiveSelf fun(self: GameObject): boolean
---@field GetActiveInHierarchy fun(self: GameObject): boolean
---@field GetIsStatic fun(self: GameObject): boolean
---@field GetTag fun(self: GameObject): string
---@field GetTransform fun(self: GameObject): Transform
---@field GetComponent fun(self: GameObject, type: Class): userdata
---@field GetComponentInChildren fun(self: GameObject, type: Class): userdata
---@field GetComponentInParent fun(self: GameObject, type: Class): userdata
---@field GetComponents fun(self: GameObject, type: Class): table<number, userdata>
---@field GetComponentsInChildren fun(self: GameObject, type: Class, includeInactive?: boolean): table<number, userdata>
---@field GetComponentsInParent fun(self: GameObject, type: Class, includeInactive?: boolean): table<number, userdata>
GameObject = {}

---@class Transform : Component
---@field GetPosition fun(self: Transform): Vector3
---@field SetPosition fun(self: Transform, pos: Vector3)
---@field GetRotation fun(self: Transform): Quaternion
---@field SetRotation fun(self: Transform, q: Quaternion)
---@field GetLocalPosition fun(self: Transform): Vector3
---@field SetLocalPosition fun(self: Transform, pos: Vector3)
---@field GetLocalRotation fun(self: Transform): Quaternion
---@field SetLocalRotation fun(self: Transform, q: Quaternion)
---@field GetLocalScale fun(self: Transform): Vector3
---@field SetLocalScale fun(self: Transform, s: Vector3)
---@field GetForward fun(self: Transform): Vector3
---@field SetForward fun(self: Transform, v: Vector3)
---@field GetRight fun(self: Transform): Vector3
---@field SetRight fun(self: Transform, v: Vector3)
---@field GetUp fun(self: Transform): Vector3
---@field SetUp fun(self: Transform, v: Vector3)
---@field GetLossyScale fun(self: Transform): Vector3
---@field TransformPoint fun(self: Transform, pos: Vector3): Vector3
---@field LookAt fun(self: Transform, worldPosition: Vector3)
---@field Rotate fun(self: Transform, eulers: Vector3)
---@field GetChildCount fun(self: Transform): number
---@field GetChild fun(self: Transform, index: number): Transform
---@field GetParent fun(self: Transform): Transform
---@field GetRoot fun(self: Transform): Transform
---@field GetGameObject fun(self: Transform): GameObject
Transform = {}

---@class Camera : Component
---@field GetDepth fun(self: Camera): number
---@field SetDepth fun(self: Camera, depth: number)
---@field GetFoV fun(self: Camera): number
---@field SetFoV fun(self: Camera, fov: number)
---@field GetAllCount fun(self: Camera): number
---@field WorldToScreenPoint fun(self: Camera, pos: Vector3): Vector3
---@field ScreenToWorldPoint fun(self: Camera, pos: Vector3): Vector3
---@field ScreenPointToRay fun(self: Camera, pos: Vector2): Ray
---@field CameraToWorldMatrix fun(self: Camera): Matrix4x4
Camera = {}

---@class Light : Component
---@field GetIntensity fun(self: Light): number
---@field SetIntensity fun(self: Light, v: number)
---@field GetColor fun(self: Light): Color
---@field SetColor fun(self: Light, c: Color)
---@field GetRange fun(self: Light): number
---@field SetRange fun(self: Light, v: number)
---@field GetLightType fun(self: Light): number
---@field SetLightType fun(self: Light, type: number)
---@field GetSpotAngle fun(self: Light): number
---@field SetSpotAngle fun(self: Light, angle: number)
---@field GetShadows fun(self: Light): number
---@field SetShadows fun(self: Light, shadowType: number)
Light = {}

---@class Rigidbody : Component
---@field GetVelocity fun(self: Rigidbody): Vector3
---@field SetVelocity fun(self: Rigidbody, v: Vector3)
---@field GetDetectCollisions fun(self: Rigidbody): boolean
---@field SetDetectCollisions fun(self: Rigidbody, v: boolean)
Rigidbody = {}

---@class Collider : Component
---@field GetBounds fun(self: Collider): Bounds
Collider = {}

---@class CapsuleCollider : Collider
---@field GetCenter fun(self: CapsuleCollider): Vector3
---@field GetDirection fun(self: CapsuleCollider): Vector3
---@field GetHeight fun(self: CapsuleCollider): Vector3
---@field GetRadius fun(self: CapsuleCollider): Vector3
CapsuleCollider = {}

---@class BoxCollider : Collider
---@field GetCenter fun(self: BoxCollider): Vector3
---@field GetSize fun(self: BoxCollider): Vector3
BoxCollider = {}

---@class Mesh : UnityObject
---@field GetBounds fun(self: Mesh): Bounds
Mesh = {}

---@class Renderer : Component
---@field GetEnabled fun(self: Renderer): boolean
---@field SetEnabled fun(self: Renderer, enable: boolean)
---@field GetMaterialCount fun(self: Renderer): number
---@field GetMaterial fun(self: Renderer): Material
---@field GetSharedMaterial fun(self: Renderer): Material
---@field SetMaterial fun(self: Renderer, material: Material)
---@field GetBounds fun(self: Renderer): Bounds
Renderer = {}

---@class Sprite : Object
---@field GetBounds fun(self: Sprite): Bounds
---@field GetRect fun(self: Sprite): Rect
---@field GetBorder fun(self: Sprite): Vector4
---@field GetTexture fun(self: Sprite): Texture2D
---@field GetPixelsPerUnit fun(self: Sprite): number
---@field GetPivot fun(self: Sprite): Vector2
---@field GetPacked fun(self: Sprite): boolean
Sprite = {}

---@class Shader : Object
---@field isSupported fun(self: Shader): boolean
---@field GetPropertyCount fun(self: Shader): number
---@field FindPropertyIndex fun(self: Shader, propertyName: string): number
---@field GetPropertyName fun(self: Shader, propertyIndex: number): string
Shader = {}

---@class Material : Object
---@field GetShader fun(self: Material): Shader
---@field SetShader fun(self: Material, shader: Shader)
---@field SetTexture fun(self: Material, name: string, value: Texture)
---@field SetTextureByID fun(self: Material, nameID: number, value: Texture)
---@field GetTexture fun(self: Material, name: string): Texture
---@field GetTextureByID fun(self: Material, nameID: number): Texture
---@field SetColor fun(self: Material, name: string, value: Color)
---@field SetColorByID fun(self: Material, nameID: number, value: Color)
---@field SetFloat fun(self: Material, name: string, value: number)
---@field SetFloatByID fun(self: Material, nameID: number, value: number)
---@field SetInt fun(self: Material, name: string, value: number)
---@field SetIntByID fun(self: Material, nameID: number, value: number)
Material = {}

---@class Texture : Object
---@field GetWidth fun(self: Texture): number
---@field GetHeight fun(self: Texture): number
---@field GetMipmapCount fun(self: Texture): number
---@field GetIsReadable fun(self: Texture): boolean
---@field GetWrapMode fun(self: Texture): number
---@field SetWrapMode fun(self: Texture, wrapMode: number)
---@field GetFilterMode fun(self: Texture): number
---@field SetFilterMode fun(self: Texture, filterMode: number)
Texture = {}

---@class Texture2D : Texture
---@field LoadRawTextureData fun(self: Texture2D, data: userdata)
---@field LoadImage fun(self: Texture2D, data: userdata)
---@field Apply fun(self: Texture2D, updateMipmaps?: boolean, makeNoLongerReadable?: boolean)
Texture2D = {}

---@class Animator : Behaviour
---@field GetBoneTransform fun(self: Animator, boneId: number): Transform
Animator = {}

---@class LayerMask
---@field m_Mask number
LayerMask = {}

-- ============================================
-- Enums
-- ============================================

Unity.LightType = {
    Spot = 0,
    Directional = 1,
    Point = 2,
    Area = 3,
    Rectangle = 3,
    Disc = 4,
    Pyramid = 5,
    Box = 6,
    Tube = 7
}

Unity.LightShadows = {
    None = 0,
    Hard = 1,
    Soft = 2
}

Unity.HumanBodyBones = {
    Hips = 0,
    LeftUpperLeg = 1,
    RightUpperLeg = 2,
    LeftLowerLeg = 3,
    RightLowerLeg = 4,
    LeftFoot = 5,
    RightFoot = 6,
    Spine = 7,
    Chest = 8,
    UpperChest = 54,
    Neck = 9,
    Head = 10,
    LeftShoulder = 11,
    RightShoulder = 12,
    LeftUpperArm = 13,
    RightUpperArm = 14,
    LeftLowerArm = 15,
    RightLowerArm = 16,
    LeftHand = 17,
    RightHand = 18,
    LeftToes = 19,
    RightToes = 20,
    LeftEye = 21,
    RightEye = 22,
    Jaw = 23,
    LeftThumbProximal = 24,
    LeftThumbIntermediate = 25,
    LeftThumbDistal = 26,
    LeftIndexProximal = 27,
    LeftIndexIntermediate = 28,
    LeftIndexDistal = 29,
    LeftMiddleProximal = 30,
    LeftMiddleIntermediate = 31,
    LeftMiddleDistal = 32,
    LeftRingProximal = 33,
    LeftRingIntermediate = 34,
    LeftRingDistal = 35,
    LeftLittleProximal = 36,
    LeftLittleIntermediate = 37,
    LeftLittleDistal = 38,
    RightThumbProximal = 39,
    RightThumbIntermediate = 40,
    RightThumbDistal = 41,
    RightIndexProximal = 42,
    RightIndexIntermediate = 43,
    RightIndexDistal = 44,
    RightMiddleProximal = 45,
    RightMiddleIntermediate = 46,
    RightMiddleDistal = 47,
    RightRingProximal = 48,
    RightRingIntermediate = 49,
    RightRingDistal = 50,
    RightLittleProximal = 51,
    RightLittleIntermediate = 52,
    RightLittleDistal = 53,
    LastBone = 55
}

-- ============================================
-- Static Namespaces
-- ============================================

---@class Unity
Unity = {}

---@class UnityGameObject
---@field Find fun(name: string): GameObject
---@field FindGameObjectsWithTag fun(tag: string): table<number, GameObject>
---@field Create fun(name: string): GameObject
Unity.GameObject = {}

---@class UnityCamera
---@field GetMain fun(): Camera
---@field GetCurrent fun(): Camera
---@field GetAllCamera fun(): table<number, Camera>
Unity.Camera = {}

---@class UnityLight
---@field FindAll fun(): table<number, Light>
Unity.Light = {}

---@class UnityString
---@field New fun(str: string): String
Unity.String = {}

---@class UnityObject_NS
---@field FindObjectsOfType fun(className: string): table<number, userdata>
---@field FindObjectsByType fun(className: string): table<number, userdata>
---@field Instantiate fun(original: UnityObject): UnityObject
---@field Destroy fun(obj: UnityObject)
---@field FindObjectFromInstanceID fun(instanceID: number): UnityObject
---@field DontDestroyOnLoad fun(target: UnityObject)
Unity.Object = {}

---@class UnityPhysics
---@field Raycast fun(origin: Vector3, direction: Vector3, maxDistance: number): boolean
---@field Linecast fun(start: Vector3, end_pos: Vector3): boolean
---@field IgnoreCollision fun(collider1: Collider, collider2: Collider)
Unity.Physics = {}

---@class UnityTime
---@field GetTime fun(): number
---@field GetDeltaTime fun(): number
---@field GetFixedDeltaTime fun(): number
---@field GetTimeScale fun(): number
---@field SetTimeScale fun(v: number)
Unity.Time = {}

---@class UnityScreen
---@field GetWidth fun(): number
---@field GetHeight fun(): number
Unity.Screen = {}

---@class UnityInput
---@field GetKey fun(key: number): boolean
---@field GetKeyDown fun(key: number): boolean
---@field GetMouseButton fun(button: number): boolean
---@field GetMouseButtonDown fun(button: number): boolean
---@field GetMousePos fun(): Vector2
---@field GetMouseDelta fun(): Vector2
---@field GetMouseWheel fun(): number
Unity.Input = {}

---@class UnityLayerMask
---@field NameToLayer fun(name: string): number
---@field LayerToName fun(layer: number): string
Unity.LayerMask = {}

---@class UnityShader
---@field Find fun(name: string): Shader
---@field EnableKeyword fun(keyword: string)
---@field DisableKeyword fun(keyword: string)
---@field PropertyToID fun(name: string): number
---@field SetGlobalFloat fun(name: string, value: number)
---@field SetGlobalVector fun(name: string, value: Vector4)
---@field SetGlobalTexture fun(name: string, value: Texture)
---@field GetGlobalFloat fun(name: string): number
Unity.Shader = {}

---@class UnityTexture2D
---@field New fun(width: number, height: number, textureFormat?: number, mipChain?: boolean): Texture2D
---@field GetWhiteTexture fun(): Texture2D
---@field GetBlackTexture fun(): Texture2D
Unity.Texture2D = {}

---@class UnitySprite
---@field Create fun(texture: Texture2D, rect: Rect, pivot: Vector2, pixelsPerUnit: number): Sprite
Unity.Sprite = {}

---@class UnityResolve
---@field GetClass fun(assembly: string, className: string, namespaze: string): Class
---@field GetAssemblies fun(): table<number, Assembly>
Unity.Resolve = {}

-- ============================================
-- Reflection Types
-- ============================================

---@class Field
---@field name string
---@field offset number
---@field static boolean
---@field typeName string
Field = {}

---@class Method
---@field name string
---@field static boolean
---@field returnType string
---@field argCount number
Method = {}

---@class Class
---@field name string
---@field namespaze string
---@field parent string
---@field GetFields fun(self: Class): table<number, Field>
---@field GetMethods fun(self: Class): table<number, Method>
---@field FindObjectsOfType fun(self: Class): table<number, Component>
---@field FindObjectsByType fun(self: Class): table<number, Component>
---@field New fun(self: Class): userdata
---@field GetFieldValue fun(self: Class, obj: userdata, fieldName: string): any
---@field SetFieldValue fun(self: Class, obj: userdata, fieldName: string, value: any)
---@field InvokeMethod fun(self: Class, obj: userdata, methodName: string, ...): any
Class = {}

---@class Assembly
---@field name string
---@field file string
Assembly = {}

-- ============================================
-- ImGui
-- ============================================

---@class DrawList
---@field AddLine fun(self: DrawList, a: Vector2, b: Vector2, col: number, thickness?: number)
---@field AddRect fun(self: DrawList, a: Vector2, b: Vector2, col: number, rounding?: number, flags?: number, thickness?: number)
---@field AddRectFilled fun(self: DrawList, a: Vector2, b: Vector2, col: number, rounding?: number, flags?: number)
---@field AddCircle fun(self: DrawList, center: Vector2, radius: number, col: number, segments?: number, thickness?: number)
---@field AddCircleFilled fun(self: DrawList, center: Vector2, radius: number, col: number, segments?: number)
---@field AddText fun(self: DrawList, pos: Vector2, col: number, text: string)
---@field AddTriangle fun(self: DrawList, a: Vector2, b: Vector2, c: Vector2, col: number, thickness?: number)
---@field AddTriangleFilled fun(self: DrawList, a: Vector2, b: Vector2, c: Vector2, col: number)
---@field PushClipRect fun(self: DrawList, a: Vector2, b: Vector2, intersect: boolean)
---@field PopClipRect fun(self: DrawList)
DrawList = {}

---@class imgui
imgui = {}

---@param name string
---@param open? boolean
---@param flags? number
function imgui.Begin(name, open, flags) end

function imgui.End() end

---@param text string
function imgui.Text(text) end

---@param col Color
---@param text string
function imgui.TextColored(col, text) end

---@param label string
---@param size? Vector2
---@return boolean
function imgui.Button(label, size) end

---@param label string
---@return boolean
function imgui.SmallButton(label) end

---@param label string
---@param v boolean
---@return boolean
function imgui.Checkbox(label, v) end

---@param label string
---@param v number
---@param min number
---@param max number
---@return number
function imgui.SliderFloat(label, v, min, max) end

---@param label string
---@param v number
---@param min number
---@param max number
---@return number
function imgui.SliderInt(label, v, min, max) end

---@param label string
---@param text string
---@param flags? number
---@return string
function imgui.InputText(label, text, flags) end

---@param label string
---@param v number
---@return number
function imgui.InputFloat(label, v) end

---@param label string
---@param v Vector3
---@return Vector3
function imgui.InputFloat3(label, v) end

function imgui.Separator() end
function imgui.Spacing() end

---@param offset? number
function imgui.SameLine(offset) end

---@param size Vector2
---@param cond? number
function imgui.SetNextWindowSize(size, cond) end

---@param pos Vector2
---@param cond? number
function imgui.SetNextWindowPos(pos, cond) end

---@param key number
---@return boolean
function imgui.IsKeyDown(key) end

---@param button number
---@return boolean
function imgui.IsMouseDown(button) end

---@return Vector2
function imgui.GetMousePos() end

---@return DrawList
function imgui.GetBackgroundDrawList() end

---@return DrawList
function imgui.GetForegroundDrawList() end

---@param label string
---@param currentItem number
---@param items string Semicolon-separated items (e.g., "A;B;C")
---@return number
function imgui.Combo(label, currentItem, items) end

---@param label string
---@param currentItem number
---@param items table<number, string>
---@return number
function imgui.ListBox(label, currentItem, items) end

---@param label string
---@param v number
---@param v_speed number
---@param v_min number
---@param v_max number
---@return number
function imgui.DragFloat(label, v, v_speed, v_min, v_max) end

---@param label string
---@param v number
---@param v_speed number
---@param v_min number
---@param v_max number
---@return number
function imgui.DragInt(label, v, v_speed, v_min, v_max) end

---@param label string
---@param c Color
---@return Color
function imgui.ColorEdit3(label, c) end

---@param label string
---@param c Color
---@return Color
function imgui.ColorEdit4(label, c) end

---@param label string
---@param selected boolean
---@return boolean
function imgui.Selectable(label, selected) end

---@param label string
---@param active boolean
---@return boolean
function imgui.RadioButton(label, active) end

---@return DrawList
function imgui.GetWindowDrawList() end

---@param idx number
---@param col Color
function imgui.PushStyleColor(idx, col) end

---@param count number
function imgui.PopStyleColor(count) end

---@param idx number
---@param val number
function imgui.PushStyleVar(idx, val) end

---@param count number
function imgui.PopStyleVar(count) end

---@return Vector2
function imgui.GetWindowSize() end

---@return Vector2
function imgui.GetWindowPos() end

---@return Vector2
function imgui.GetContentRegionAvail() end

---@return number
function imgui.GetFrameHeight() end

---@return number
function imgui.GetFrameHeightWithSpacing() end

---@return boolean
function imgui.IsItemHovered() end

---@param mouseButton number
---@return boolean
function imgui.IsItemClicked(mouseButton) end

---@param text string
function imgui.SetTooltip(text) end

function imgui.BeginTooltip() end
function imgui.EndTooltip() end

---@param id string
---@param size Vector2
---@param border boolean
---@param flags number
function imgui.BeginChild(id, size, border, flags) end

function imgui.EndChild() end

---@param label string
---@param open boolean
---@return boolean
function imgui.CollapsingHeader(label, open) end

---@param label string
---@return boolean
function imgui.TreeNode(label) end

function imgui.TreePop() end
function imgui.Indent() end
function imgui.Unindent() end

---@param text string
function imgui.BulletText(text) end

---@param count number
---@param id string
---@param border boolean
function imgui.Columns(count, id, border) end

function imgui.NextColumn() end

---@param index number
---@param width number
function imgui.SetColumnWidth(index, width) end

-- ============================================
-- Logging
-- ============================================

---@class log
log = {}

---@param msg string
function log.info(msg) end

---@param msg string
function log.warn(msg) end

---@param msg string
function log.error(msg) end

---@param msg string
function log.debug(msg) end

---@param ... any
function print(...) end

-- ============================================
-- Plugin Lifecycle Callbacks
-- ============================================

---@param dt number Delta time in seconds
function onUpdate(dt) end

function onInit() end
function onRender() end
function onUnload() end
