#include "StockFlowApp.h"
#include <cstring>
#include <cstdio>

void StockFlowApp::RenderAdminScreen()
{
    RenderTopBar();
    RenderNotifPanel();

    ImGuiIO& io = ImGui::GetIO();
    float W = io.DisplaySize.x, H = io.DisplaySize.y;

    ImGui::SetNextWindowPos(ImVec2(0, 54));
    ImGui::SetNextWindowSize(ImVec2(W, H - 54));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, COL_BG_DARK);
    ImGui::Begin("##admin", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    ImGui::Spacing();
    ImGui::SetWindowFontScale(1.5f);
    ImGui::TextColored(COL_WHITE, "  Admin Control Panel");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextColored(COL_MUTED, "  Manage inventory and stock");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Stats
    int total = inventory->getCount();
    int outStock = 0, lowStock = 0;
    double invVal = 0;
    for (int i = 0; i < total; i++) {
        Product* p = inventory->getProductAt(i);
        if (p->getStock() == 0)       outStock++;
        else if (p->getStock() <= 5)  lowStock++;
        invVal += p->getPrice() * p->getStock();
    }

    char sBuf[32];
    float cardW = (W - 32 - 36) / 4.0f;

    snprintf(sBuf, sizeof(sBuf), "%d", total);
    RenderStatCard("Total Products", sBuf, COL_ACCENT, cardW);
    ImGui::SameLine(0, 12);

    snprintf(sBuf, sizeof(sBuf), "%d", lowStock);
    RenderStatCard("Low Stock", sBuf, COL_GOLD, cardW);
    ImGui::SameLine(0, 12);

    snprintf(sBuf, sizeof(sBuf), "%d", outStock);
    RenderStatCard("Out of Stock", sBuf, COL_RED, cardW);
    ImGui::SameLine(0, 12);

    snprintf(sBuf, sizeof(sBuf), "$%.0f", invVal);
    RenderStatCard("Inventory Value", sBuf, COL_GREEN, cardW);

    ImGui::Spacing(); ImGui::Spacing();

    // Tabs
    if (ImGui::BeginTabBar("##admintabs")) {
        if (ImGui::BeginTabItem("  Inventory  ")) {
            RenderAdminInventoryTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("  Add Product  ")) {
            RenderAdminAddProductTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("  Update Stock  ")) {
            RenderAdminUpdateStockTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    // Remove confirm popup
    if (showRemoveConfirm)
        ImGui::OpenPopup("Confirm Remove");

    ImGui::SetNextWindowSize(ImVec2(360, 170));
    if (ImGui::BeginPopupModal("Confirm Remove", nullptr,
        ImGuiWindowFlags_NoResize))
    {
        ImGui::Spacing();
        ImGui::TextColored(COL_RED, "  Remove Product?");
        ImGui::Spacing();
        ImGui::TextWrapped("  Remove \"%s\" (ID: %d)?",
            removeTargetName.c_str(), removeTargetId);
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button,
            ImVec4(0.55f, 0.12f, 0.12f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
            ImVec4(0.72f, 0.18f, 0.18f, 1.f));
        if (ImGui::Button("  Yes, Remove  ", ImVec2(150, 34))) {
            inventory->removeItem(removeTargetId);
            AddNotification("Removed: " + removeTargetName, 3);
            showRemoveConfirm = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine(0, 10);
        if (ImGui::Button("  Cancel  ", ImVec2(110, 34))) {
            showRemoveConfirm = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
    ImGui::PopStyleColor();
}

void StockFlowApp::RenderAdminInventoryTab()
{
    ImGui::Spacing();
    ImGui::Text("Search:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(220);
    ImGui::InputText("##srch", apSearchFilter, sizeof(apSearchFilter));
    ImGui::Spacing();

    ImGuiIO& io = ImGui::GetIO();
    float tableH = io.DisplaySize.y - 300;

    ImGuiTableFlags flags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("##inv", 6, flags, ImVec2(-1, tableH)))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 60);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableSetupColumn("Stock", ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 110);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableHeadersRow();

        string filter = string(apSearchFilter);
        for (int i = 0; i < inventory->getCount(); i++) {
            Product* p = inventory->getProductAt(i);
            string nm = p->getName();
            if (!filter.empty() &&
                nm.find(filter) == string::npos &&
                to_string(p->getId()).find(filter) == string::npos)
                continue;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(COL_MUTED, "#%d", p->getId());

            ImGui::TableSetColumnIndex(1);
            string dn = nm;
            for (char& c : dn) if (c == '_') c = ' ';
            ImGui::Text("%s", dn.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::TextColored(COL_GOLD, "$%.2f", p->getPrice());

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%d", p->getStock());

            ImGui::TableSetColumnIndex(4);
            if (p->getStock() == 0)
                ImGui::TextColored(COL_RED, "Out of Stock");
            else if (p->getStock() <= 5)
                ImGui::TextColored(COL_GOLD, "Low Stock");
            else
                ImGui::TextColored(COL_GREEN, "In Stock");

            ImGui::TableSetColumnIndex(5);
            char rmLabel[32];
            snprintf(rmLabel, sizeof(rmLabel), "Remove##%d", p->getId());
            ImGui::PushStyleColor(ImGuiCol_Button,
                ImVec4(0.35f, 0.10f, 0.10f, 1.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                ImVec4(0.55f, 0.16f, 0.16f, 1.f));
            if (ImGui::SmallButton(rmLabel)) {
                removeTargetId = p->getId();
                removeTargetName = nm;
                showRemoveConfirm = true;
            }
            ImGui::PopStyleColor(2);
        }
        ImGui::EndTable();
    }
}

void StockFlowApp::RenderAdminAddProductTab()
{
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
    ImGui::BeginChild("##addform", ImVec2(500, 290), true);

    ImGui::Spacing();
    ImGui::TextColored(COL_ACCENT, "  New Product Details");
    ImGui::Separator(); ImGui::Spacing();

    ImGui::Text("Product ID");
    ImGui::SetNextItemWidth(460);
    ImGui::InputInt("##apid", &apId);

    ImGui::Spacing();
    ImGui::Text("Product Name (use underscores, no spaces)");
    ImGui::SetNextItemWidth(460);
    ImGui::InputText("##apname", apName, sizeof(apName));

    ImGui::Spacing();
    ImGui::Text("Price ($)");
    ImGui::SetNextItemWidth(460);
    ImGui::InputFloat("##apprice", &apPrice, 0.5f, 5.0f, "%.2f");

    ImGui::Spacing();
    ImGui::Text("Initial Stock");
    ImGui::SetNextItemWidth(460);
    ImGui::InputInt("##apstock", &apStock);

    ImGui::Spacing(); ImGui::Spacing();

    static string apErr, apOk;
    ImGui::PushStyleColor(ImGuiCol_Button,
        ImVec4(0.30f, 0.22f, 0.60f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
        ImVec4(0.42f, 0.32f, 0.80f, 1.f));
    if (ImGui::Button("  Add Product  ", ImVec2(460, 38))) {
        apErr.clear(); apOk.clear();
        if (apId <= 0)              apErr = "Invalid ID.";
        else if (strlen(apName) == 0) apErr = "Name cannot be empty.";
        else if (apPrice <= 0)      apErr = "Price must be positive.";
        else if (apStock < 0)       apErr = "Stock cannot be negative.";
        else if (inventory->findProduct(apId))
            apErr = "ID already exists!";
        else {
            inventory->addProduct(
                new Product(apId, string(apName),
                    (double)apPrice, apStock));
            AddNotification("Added: " + string(apName), 1);
            apOk = "Product added successfully!";
            apId = 0; apPrice = 0; apStock = 0;
            memset(apName, 0, sizeof(apName));
        }
    }
    ImGui::PopStyleColor(2);

    if (!apErr.empty()) ImGui::TextColored(COL_RED, " %s", apErr.c_str());
    if (!apOk.empty())  ImGui::TextColored(COL_GREEN, " %s", apOk.c_str());

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void StockFlowApp::RenderAdminUpdateStockTab()
{
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
    ImGui::BeginChild("##updform", ImVec2(500, 230), true);

    ImGui::Spacing();
    ImGui::TextColored(COL_ACCENT, "  Update Stock");
    ImGui::Separator(); ImGui::Spacing();

    ImGui::Text("Product ID");
    ImGui::SetNextItemWidth(460);
    ImGui::InputInt("##upid", &upId);

    ImGui::Spacing();
    ImGui::Text("Quantity Change (+restock / -reduce)");
    ImGui::SetNextItemWidth(460);
    ImGui::InputInt("##upqty", &upQty);

    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "Quick:");
    ImGui::SameLine();
    if (ImGui::SmallButton("+10")) upQty = 10;  ImGui::SameLine();
    if (ImGui::SmallButton("+50")) upQty = 50;  ImGui::SameLine();
    if (ImGui::SmallButton("-1"))  upQty = -1;  ImGui::SameLine();
    if (ImGui::SmallButton("-5"))  upQty = -5;

    ImGui::Spacing(); ImGui::Spacing();

    static string upErr, upOk;
    ImGui::PushStyleColor(ImGuiCol_Button,
        ImVec4(0.30f, 0.22f, 0.60f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
        ImVec4(0.42f, 0.32f, 0.80f, 1.f));
    if (ImGui::Button("  Apply Update  ", ImVec2(460, 38))) {
        upErr.clear(); upOk.clear();
        Product* p = inventory->findProduct(upId);
        if (!p) {
            upErr = "Product ID not found.";
        }
        else if (p->getStock() + upQty < 0) {
            upErr = "Stock cannot go below 0!";
        }
        else {
            inventory->updateStock(upId, upQty);
            AddNotification(
                (upQty > 0 ? "Restocked " : "Reduced ") +
                p->getName(), upQty > 0 ? 1 : 2);
            upOk = "Updated! New stock: " +
                to_string(p->getStock());
            upId = 0; upQty = 0;
        }
    }
    ImGui::PopStyleColor(2);

    if (!upErr.empty()) ImGui::TextColored(COL_RED, " %s", upErr.c_str());
    if (!upOk.empty())  ImGui::TextColored(COL_GREEN, " %s", upOk.c_str());

    ImGui::EndChild();
    ImGui::PopStyleColor();
}
