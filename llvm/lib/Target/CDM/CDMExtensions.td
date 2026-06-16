class CDMExtension<string name, string desc, list<SubtargetFeature> implies = []>
    : SubtargetFeature<name, !subst("Extension", "HasExt", NAME), "true", desc, implies>;

def ExtensionM : CDMExtension<"M", "Integer multiplication and division">;
def ExtensionE : CDMExtension<"E", "Operating system support">;
