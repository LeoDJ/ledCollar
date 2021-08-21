# only needed because PlatformIO doesn't allow link flags directly into the .ini for whatever reason
Import("env")

env.Append(
  LINKFLAGS=[
      "--specs=nosys.specs",
      "--specs=nano.specs"
  ]
)