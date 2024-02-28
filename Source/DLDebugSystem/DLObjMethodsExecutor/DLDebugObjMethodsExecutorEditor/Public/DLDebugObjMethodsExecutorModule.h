#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FDLDebugObjMethodsExecutorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OpenDebugTabButtonClicked() const;

	void AddToolbarExtension(FToolBarBuilder& Builder) const;

private:
	TSharedPtr<class FUICommandList> EditorCommands;

	TSharedPtr<class FDLDebugEditor> DLDebugEditor;
};



IMPLEMENT_MODULE(FDLDebugObjMethodsExecutorModule, DLDebugObjMethodsExecutorEditor);
