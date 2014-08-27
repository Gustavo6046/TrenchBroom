/*
 Copyright (C) 2010-2014 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ModelFilterPopupEditor.h"

#include "Model/Game.h"
#include "Model/ModelFilter.h"
#include "Renderer/RenderConfig.h"
#include "View/BorderPanel.h"
#include "View/MapDocument.h"
#include "View/PopupButton.h"
#include "View/TitledPanel.h"
#include "View/ViewConstants.h"
#include "View/wxUtils.h"

#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

namespace TrenchBroom {
    namespace View {
        ModelFilterEditor::ModelFilterEditor(wxWindow* parent, MapDocumentWPtr document) :
        wxPanel(parent),
        m_document(document) {
            bindObservers();
        }
        
        ModelFilterEditor::~ModelFilterEditor() {
            unbindObservers();
        }
        
        void ModelFilterEditor::OnShowEntityClassnamesChanged(wxCommandEvent& event) {
            MapDocumentSPtr document = lock(m_document);
            Renderer::RenderConfig& config = document->renderConfig();
            config.setShowEntityClassnames(event.IsChecked());
        }
        
        void ModelFilterEditor::OnShowEntityBoundsChanged(wxCommandEvent& event) {
            MapDocumentSPtr document = lock(m_document);
            Renderer::RenderConfig& config = document->renderConfig();
            config.setShowEntityBounds(event.IsChecked());
        }
        
        void ModelFilterEditor::OnShowPointEntitiesChanged(wxCommandEvent& event) {
            MapDocumentSPtr document = lock(m_document);
            Model::ModelFilter& filter = document->filter();
            filter.setShowPointEntities(event.IsChecked());
        }

        void ModelFilterEditor::OnShowPointEntityModelsChanged(wxCommandEvent& event) {
            MapDocumentSPtr document = lock(m_document);
            Renderer::RenderConfig& config = document->renderConfig();
            config.setShowPointEntityModels(event.IsChecked());
        }

        void ModelFilterEditor::OnShowBrushesChanged(wxCommandEvent& event) {
            MapDocumentSPtr document = lock(m_document);
            Model::ModelFilter& filter = document->filter();
            filter.setShowBrushes(event.IsChecked());
        }
        
        void ModelFilterEditor::OnShowBrushContentTypeChanged(wxCommandEvent& event) {
            MapDocumentSPtr document = lock(m_document);
            Model::GamePtr game = document->game();
            
            if (game != NULL) {
                Model::BrushContentType::FlagType hiddenFlags = 0;
                const Model::BrushContentType::List& contentTypes = game->brushContentTypes();
                
                for (size_t i = 0; i < contentTypes.size(); ++i) {
                    const Model::BrushContentType& contentType = contentTypes[i];
                    wxCheckBox* checkBox = m_brushContentTypeCheckBoxes[i];
                    if (!checkBox->GetValue())
                        hiddenFlags |= contentType.flagValue();
                }
                
                Model::ModelFilter& filter = document->filter();
                filter.setHiddenBrushContentTypes(hiddenFlags);
            }
        }

        void ModelFilterEditor::OnFaceRenderModeChanged(wxCommandEvent& event) {
            MapDocumentSPtr document = lock(m_document);
            Renderer::RenderConfig& config = document->renderConfig();
            
            switch (event.GetSelection()) {
                case 1:
                    config.setFaceRenderMode(Renderer::RenderConfig::FaceRenderMode_Flat);
                    break;
                case 2:
                    config.setFaceRenderMode(Renderer::RenderConfig::FaceRenderMode_Skip);
                    break;
                default:
                    config.setFaceRenderMode(Renderer::RenderConfig::FaceRenderMode_Textured);
                    break;
            }
        }
        
        void ModelFilterEditor::OnShadeFacesChanged(wxCommandEvent& event) {
            MapDocumentSPtr document = lock(m_document);
            Renderer::RenderConfig& config = document->renderConfig();
            config.setShadeFaces(event.IsChecked());
        }
        
        void ModelFilterEditor::OnUseFogChanged(wxCommandEvent& event) {
            MapDocumentSPtr document = lock(m_document);
            Renderer::RenderConfig& config = document->renderConfig();
            config.setUseFog(event.IsChecked());
        }
        
        void ModelFilterEditor::OnShowEdgesChanged(wxCommandEvent& event) {
            MapDocumentSPtr document = lock(m_document);
            Renderer::RenderConfig& config = document->renderConfig();
            config.setShowEdges(event.IsChecked());
        }

        void ModelFilterEditor::bindObservers() {
            MapDocumentSPtr document = lock(m_document);
            document->documentWasNewedNotifier.addObserver(this, &ModelFilterEditor::documentWasNewedOrLoaded);
            document->documentWasLoadedNotifier.addObserver(this, &ModelFilterEditor::documentWasNewedOrLoaded);
            document->modelFilterDidChangeNotifier.addObserver(this, &ModelFilterEditor::modelFilterDidChange);
            document->renderConfigDidChangeNotifier.addObserver(this, &ModelFilterEditor::renderConfigDidChange);
        }
        
        void ModelFilterEditor::unbindObservers() {
            if (!expired(m_document)) {
                MapDocumentSPtr document = lock(m_document);
                document->documentWasNewedNotifier.removeObserver(this, &ModelFilterEditor::documentWasNewedOrLoaded);
                document->documentWasLoadedNotifier.removeObserver(this, &ModelFilterEditor::documentWasNewedOrLoaded);
                document->modelFilterDidChangeNotifier.removeObserver(this, &ModelFilterEditor::modelFilterDidChange);
                document->renderConfigDidChangeNotifier.removeObserver(this, &ModelFilterEditor::renderConfigDidChange);
            }
        }
        
        void ModelFilterEditor::documentWasNewedOrLoaded() {
            createGui();
            refreshGui();
        }
        
        void ModelFilterEditor::modelFilterDidChange() {
            refreshGui();
        }

        void ModelFilterEditor::renderConfigDidChange() {
            refreshGui();
        }

        void ModelFilterEditor::createGui() {
            DestroyChildren();

            wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(createEntitiesPanel(), 0, wxEXPAND);
            sizer->AddSpacer(LayoutConstants::WideVMargin);
            sizer->Add(createBrushesPanel(), 0, wxEXPAND);
            sizer->AddSpacer(LayoutConstants::WideVMargin);
            sizer->Add(createRendererPanel(), 0, wxEXPAND);
            sizer->AddSpacer(LayoutConstants::NarrowVMargin);
            SetSizerAndFit(sizer);

            GetParent()->Fit();
            GetParent()->GetParent()->Fit();
        }

        wxWindow* ModelFilterEditor::createEntitiesPanel() {
            TitledPanel* panel = new TitledPanel(this, "Entities", false);

            m_showEntityClassnamesCheckBox = new wxCheckBox(panel->getPanel(), wxID_ANY, "Show entity classnames");
            m_showEntityBoundsCheckBox = new wxCheckBox(panel->getPanel(), wxID_ANY, "Show entity bounds");
            m_showPointEntitiesCheckBox = new wxCheckBox(panel->getPanel(), wxID_ANY, "Show point entities");
            m_showPointEntityModelsCheckBox = new wxCheckBox(panel->getPanel(), wxID_ANY, "Show point entity models");
            
            m_showEntityClassnamesCheckBox->Bind(wxEVT_CHECKBOX, &ModelFilterEditor::OnShowEntityClassnamesChanged, this);
            m_showEntityBoundsCheckBox->Bind(wxEVT_CHECKBOX, &ModelFilterEditor::OnShowEntityBoundsChanged, this);
            m_showPointEntitiesCheckBox->Bind(wxEVT_CHECKBOX, &ModelFilterEditor::OnShowPointEntitiesChanged, this);
            m_showPointEntityModelsCheckBox->Bind(wxEVT_CHECKBOX, &ModelFilterEditor::OnShowPointEntityModelsChanged, this);
            
            wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(m_showEntityClassnamesCheckBox);
            sizer->Add(m_showEntityBoundsCheckBox);
            sizer->Add(m_showPointEntitiesCheckBox);
            sizer->Add(m_showPointEntityModelsCheckBox);
            
            panel->getPanel()->SetSizerAndFit(sizer);
            return panel;
        }
        
        wxWindow* ModelFilterEditor::createBrushesPanel() {
            TitledPanel* panel = new TitledPanel(this, "Brushes", false);
            wxWindow* inner = panel->getPanel();
            createBrushContentTypeFilter(inner);
            
            m_showBrushesCheckBox = new wxCheckBox(panel->getPanel(), wxID_ANY, "Show brushes");
            m_showBrushesCheckBox->Bind(wxEVT_CHECKBOX, &ModelFilterEditor::OnShowBrushesChanged, this);
            
            assert(inner->GetSizer() != NULL);
            inner->GetSizer()->Prepend(m_showBrushesCheckBox);
            
            return panel;
        }

        void ModelFilterEditor::createBrushContentTypeFilter(wxWindow* parent) {
            m_brushContentTypeCheckBoxes.clear();

            MapDocumentSPtr document = lock(m_document);
            Model::GamePtr game = document->game();
            if (game == NULL) {
                createEmptyBrushContentTypeFilter(parent);
            } else {
                const Model::BrushContentType::List& contentTypes = game->brushContentTypes();
                if (contentTypes.empty()) {
                    createEmptyBrushContentTypeFilter(parent);
                } else {
                    createBrushContentTypeFilter(parent, contentTypes);
                }
            }
        }

        void ModelFilterEditor::createEmptyBrushContentTypeFilter(wxWindow* parent) {
            wxStaticText* msg = new wxStaticText(parent, wxID_ANY, "No brush content types found");
            msg->SetForegroundColour(*wxLIGHT_GREY);
            
            wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
            sizer->AddStretchSpacer();
            sizer->AddSpacer(LayoutConstants::WideHMargin);
            sizer->Add(msg, 0, wxTOP | wxBOTTOM, LayoutConstants::NarrowVMargin);
            sizer->AddSpacer(LayoutConstants::WideHMargin);
            sizer->AddStretchSpacer();
            
            parent->SetSizerAndFit(sizer);
        }

        void ModelFilterEditor::createBrushContentTypeFilter(wxWindow* parent, const Model::BrushContentType::List& contentTypes) {
            assert(!contentTypes.empty());
            
            wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            for (size_t i = 0; i < contentTypes.size(); ++i) {
                const Model::BrushContentType& contentType = contentTypes[i];
                
                wxString label = "Show ";
                label << StringUtils::toLower(contentType.name());
                
                wxCheckBox* checkBox = new wxCheckBox(parent, wxID_ANY, label);
                m_brushContentTypeCheckBoxes.push_back(checkBox);
                
                sizer->Add(checkBox);
                checkBox->Bind(wxEVT_CHECKBOX, &ModelFilterEditor::OnShowBrushContentTypeChanged, this);
            }
            parent->SetSizerAndFit(sizer);
        }
        
        wxWindow* ModelFilterEditor::createRendererPanel() {
            TitledPanel* panel = new TitledPanel(this, "Renderer", false);
            wxWindow* inner = panel->getPanel();
            
            static const wxString FaceRenderModes[] = { "Textured", "Flat", "Hidden" };
            m_faceRenderModeChoice = new wxChoice(inner, wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, FaceRenderModes);
            m_shadeFacesCheckBox = new wxCheckBox(inner, wxID_ANY, "Shade faces");
            m_useFogCheckBox = new wxCheckBox(inner, wxID_ANY, "Use fog");
            m_showEdgesCheckBox = new wxCheckBox(inner, wxID_ANY, "Show edges");

            m_faceRenderModeChoice->Bind(wxEVT_CHOICE, &ModelFilterEditor::OnFaceRenderModeChanged, this);
            m_shadeFacesCheckBox->Bind(wxEVT_CHECKBOX, &ModelFilterEditor::OnShadeFacesChanged, this);
            m_useFogCheckBox->Bind(wxEVT_CHECKBOX, &ModelFilterEditor::OnUseFogChanged, this);
            m_showEdgesCheckBox->Bind(wxEVT_CHECKBOX, &ModelFilterEditor::OnShowEdgesChanged, this);
            
            wxSizer* choiceSizer = new wxBoxSizer(wxHORIZONTAL);
            choiceSizer->AddSpacer(LayoutConstants::ChoiceLeftMargin);
            choiceSizer->Add(m_faceRenderModeChoice);
            choiceSizer->Add(new wxStaticText(inner, wxID_ANY, "faces"), 0, wxALIGN_CENTER_VERTICAL);
            
            wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            sizer->AddSpacer(LayoutConstants::ChoiceTopMargin);
            sizer->Add(choiceSizer);
            sizer->AddSpacer(LayoutConstants::ChoiceSizeDelta);
            sizer->AddSpacer(LayoutConstants::NarrowVMargin);
            sizer->Add(m_shadeFacesCheckBox);
            sizer->Add(m_useFogCheckBox);
            sizer->Add(m_showEdgesCheckBox);
            
            inner->SetSizerAndFit(sizer);
            return panel;
        }

        void ModelFilterEditor::refreshGui() {
            refreshEntitiesPanel();
            refreshBrushesPanel();
            refreshRendererPanel();
        }

        void ModelFilterEditor::refreshEntitiesPanel() {
            MapDocumentSPtr document = lock(m_document);
            const Renderer::RenderConfig& config = document->renderConfig();
            
            m_showEntityClassnamesCheckBox->SetValue(config.showEntityClassnames());
            m_showEntityBoundsCheckBox->SetValue(config.showEntityBounds());
            m_showPointEntitiesCheckBox->SetValue(config.showPointEntities());
            m_showPointEntityModelsCheckBox->SetValue(config.showPointEntityModels());
        }

        void ModelFilterEditor::refreshBrushesPanel() {
            MapDocumentSPtr document = lock(m_document);

            const Renderer::RenderConfig& config = document->renderConfig();
            m_showBrushesCheckBox->SetValue(config.showBrushes());

            const Model::ModelFilter& filter = document->filter();
            const Model::BrushContentType::FlagType hiddenFlags = filter.hiddenBrushContentTypes();
            
            Model::GamePtr game = document->game();
            if (game != NULL) {
                const Model::BrushContentType::List& contentTypes = game->brushContentTypes();
                for (size_t i = 0; i < contentTypes.size(); ++i) {
                    const Model::BrushContentType& contentType = contentTypes[i];
                    wxCheckBox* checkBox = m_brushContentTypeCheckBoxes[i];
                    checkBox->SetValue((contentType.flagValue() & hiddenFlags) == 0);
                }
            }
        }

        void ModelFilterEditor::refreshRendererPanel() {
            MapDocumentSPtr document = lock(m_document);
            const Renderer::RenderConfig& config = document->renderConfig();
            
            m_faceRenderModeChoice->SetSelection(config.faceRenderMode());
            m_shadeFacesCheckBox->SetValue(config.shadeFaces());
            m_useFogCheckBox->SetValue(config.useFog());
            m_showEdgesCheckBox->SetValue(config.showEdges());
        }

        ModelFilterPopupEditor::ModelFilterPopupEditor(wxWindow* parent, MapDocumentWPtr document) :
        wxPanel(parent),
        m_button(NULL),
        m_editor(NULL) {
            m_button = new PopupButton(this, "View");
            m_button->SetToolTip("Click to edit view settings");
            
            BorderPanel* editorContainer = new BorderPanel(m_button->GetPopupWindow(), wxALL);
            m_editor = new ModelFilterEditor(editorContainer, document);
            
            wxSizer* containerSizer = new wxBoxSizer(wxVERTICAL);
            containerSizer->Add(m_editor, 1, wxEXPAND | wxALL, LayoutConstants::DialogOuterMargin);
            editorContainer->SetSizer(containerSizer);
            
            wxSizer* popupSizer = new wxBoxSizer(wxVERTICAL);
            popupSizer->Add(editorContainer, 1, wxEXPAND);
            m_button->GetPopupWindow()->SetSizer(popupSizer);
            
            wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
            sizer->Add(m_button, 0, wxALIGN_CENTER_VERTICAL);
            SetSizerAndFit(sizer);
        }
    }
}