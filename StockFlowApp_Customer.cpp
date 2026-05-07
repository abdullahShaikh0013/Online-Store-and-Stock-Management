#include "StockFlowApp.h"
#include <cstring>
#include <cstdio>

void StockFlowApp::RenderCustomerScreen()
{
    RenderTopBar();
    RenderNotifPanel();

    ImGuiIO& io = ImGui::GetIO();
    float W = io.DisplaySize.x, H = io.DisplaySize.y;

    ImGui::SetNextWindowPos(ImVec2(0, 54));
    ImGui::SetNextWindowSize(ImVec2(W, H - 54));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, COL_BG_DARK);
    ImGui::Begin("##cust", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    ImGui::Spacing();
    ImGui::SetWindowFontScale(1.5f);
    ImGui::TextColored(COL_WHITE, "  Store Catalog");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextColored(COL_MUTED, "  Browse and purchase products");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Stats
    int available = 0;
    for (int i = 0; i < inventory->getCount(); i++)
        if (inventory->getProductAt(i)->getStock() > 0) available++;

    char sBuf[32];
    float cardW = (W - 32 - 36) / 4.0f;

    snprintf(sBuf, sizeof(sBuf), "%d", available);
    RenderStatCard("Available", sBuf, COL_ACCENT, cardW);
    ImGui::SameLine(0, 12);

    snprintf(sBuf, sizeof(sBuf), "%d", CartItemCount());
    RenderStatCard("In Cart", sBuf, COL_GOLD, cardW);
    ImGui::SameLine(0, 12);

    snprintf(sBuf, sizeof(sBuf), "$%.2f", CartTotal());
    RenderStatCard("Cart Total", sBuf, COL_GREEN, cardW);
    ImGui::SameLine(0, 12);

    snprintf(sBuf, sizeof(sBuf), "%d", (int)orderHistory.size());
    RenderStatCard("Orders", sBuf, COL_ACCENT, cardW);

    ImGui::Spacing(); ImGui::Spacing();

    SyncCartFromBackend();

    // Tabs
    if (ImGui::BeginTabBar("##custtabs")) {
        if (ImGui::BeginTabItem("  Catalog  ")) {
            RenderCatalogTab();
            ImGui::EndTabItem();
        }
        char cartLabel[32];
        snprintf(cartLabel, sizeof(cartLabel),
            "  Cart (%d)  ", CartItemCount());
        if (ImGui::BeginTabItem(cartLabel)) {
            RenderCartTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("  Orders  ")) {
            RenderOrderHistoryTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    // Checkout success popup
    if (checkoutSuccess) ImGui::OpenPopup("Order Placed!");
    ImGui::SetNextWindowSize(ImVec2(400, 220));
    if (ImGui::BeginPopupModal("Order Placed!", nullptr,
        ImGuiWindowFlags_NoResize))
    {
        ImGui::Spacing();
        ImGui::TextColored(COL_GREEN, "  Order Placed Successfully!");
        ImGui::Spacing();
        ImGui::TextColored(COL_MUTED, "  Order #%d", lastOrder.orderId);
        ImGui::Spacing();
        ImGui::TextWrapped("  %s", lastOrder.items.c_str());
        ImGui::Spacing();
        ImGui::TextColored(COL_GOLD,
            "  Total: $%.2f via %s",
            lastOrder.total, lastOrder.payment.c_str());
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Button,
            ImVec4(0.20f, 0.45f, 0.25f, 1.f));
        if (ImGui::Button("  Continue Shopping  ", ImVec2(-1, 36))) {
            checkoutSuccess = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor();
        ImGui::EndPopup();
    }

    ImGui::End();
    ImGui::PopStyleColor();
}

void StockFlowApp::RenderCatalogTab()
{
    ImGui::Spacing();
    ImGui::Text("Search:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(220);
    ImGui::InputText("##catsrch", catSearch, sizeof(catSearch));
    ImGui::Spacing();

    ImGuiIO& io = ImGui::GetIO();
    float tableH = io.DisplaySize.y - 300;
    ImGuiTableFlags flags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("##cat", 6, flags, ImVec2(-1, tableH)))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 60);
        ImGui::TableSetupColumn("Product", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableSetupColumn("Stock", ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableSetupColumn("Qty", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableHeadersRow();

        string filter = string(catSearch);
        for (int i = 0; i < inventory->getCount(); i++) {
            Product* p = inventory->getProductAt(i);
            string nm = p->getName();
            if (!filter.empty() &&
                nm.find(filter) == string::npos &&
                to_string(p->getId()).find(filter) == string::npos)
                continue;

            bool oos = (p->getStock() == 0);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(COL_MUTED, "#%d", p->getId());

            ImGui::TableSetColumnIndex(1);
            string dn = nm;
            for (char& c : dn) if (c == '_') c = ' ';
            if (oos) ImGui::TextColored(COL_MUTED, "%s", dn.c_str());
            else     ImGui::Text("%s", dn.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::TextColored(COL_GOLD, "$%.2f", p->getPrice());

            ImGui::TableSetColumnIndex(3);
            if (oos) ImGui::TextColored(COL_RED, "N/A");
            else     ImGui::TextColored(COL_GREEN, "%d", p->getStock());

            ImGui::TableSetColumnIndex(4);
            char qLabel[32];
            snprintf(qLabel, sizeof(qLabel), "##q%d", i);
            if (oos) {
                ImGui::BeginDisabled();
                ImGui::SetNextItemWidth(65);
                ImGui::InputInt(qLabel, &cartQtyMap[i]);
                ImGui::EndDisabled();
            }
            else {
                ImGui::SetNextItemWidth(65);
                ImGui::InputInt(qLabel, &cartQtyMap[i]);
                if (cartQtyMap[i] < 1) cartQtyMap[i] = 1;
                if (cartQtyMap[i] > p->getStock())
                    cartQtyMap[i] = p->getStock();
            }

            ImGui::TableSetColumnIndex(5);
            char aLabel[32];
            snprintf(aLabel, sizeof(aLabel), "+Cart##a%d", p->getId());
            if (oos) {
                ImGui::BeginDisabled();
                ImGui::SmallButton(aLabel);
                ImGui::EndDisabled();
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Button,
                    ImVec4(0.20f, 0.35f, 0.20f, 1.f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                    ImVec4(0.26f, 0.50f, 0.28f, 1.f));
                if (ImGui::SmallButton(aLabel)) {
                    int qty = cartQtyMap[i];
                    bool found = false;
                    for (auto& ce : cartEntries) {
                        if (ce.productId == p->getId()) {
                            if (ce.qty + qty <= p->getStock())
                                ce.qty += qty;
                            found = true; break;
                        }
                    }
                    if (!found)
                        cartEntries.push_back(
                            { p->getId(), nm, p->getPrice(), qty });
                    string dn2 = nm;
                    for (char& c : dn2) if (c == '_') c = ' ';
                    AddNotification("Added " + to_string(qty) +
                        "x " + dn2, 1);
                }
                ImGui::PopStyleColor(2);
            }
        }
        ImGui::EndTable();
    }
}

void StockFlowApp::RenderCartTab()
{
    ImGui::Spacing();
    if (cartEntries.empty()) {
        ImGui::Spacing();
        ImGui::TextColored(COL_MUTED, "  Your cart is empty.");
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    float tableH = io.DisplaySize.y - 380;
    ImGuiTableFlags flags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("##cart", 5, flags, ImVec2(-1, tableH)))
    {
        ImGui::TableSetupColumn("Product", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableSetupColumn("Qty", ImGuiTableColumnFlags_WidthFixed, 55);
        ImGui::TableSetupColumn("Total", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableSetupColumn("Remove", ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableHeadersRow();

        int removeIdx = -1;
        for (int i = 0; i < (int)cartEntries.size(); i++) {
            auto& ce = cartEntries[i];
            string dn = ce.productName;
            for (char& c : dn) if (c == '_') c = ' ';

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", dn.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(COL_GOLD, "$%.2f", ce.price);
            ImGui::TableSetColumnIndex(2); ImGui::Text("x%d", ce.qty);
            ImGui::TableSetColumnIndex(3);
            ImGui::TextColored(COL_GREEN, "$%.2f", ce.total());
            ImGui::TableSetColumnIndex(4);
            char rmLabel[32];
            snprintf(rmLabel, sizeof(rmLabel), " X ##r%d", i);
            ImGui::PushStyleColor(ImGuiCol_Button,
                ImVec4(0.40f, 0.10f, 0.10f, 1.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                ImVec4(0.60f, 0.16f, 0.16f, 1.f));
            if (ImGui::SmallButton(rmLabel)) removeIdx = i;
            ImGui::PopStyleColor(2);
        }
        if (removeIdx >= 0)
            cartEntries.erase(cartEntries.begin() + removeIdx);
        ImGui::EndTable();
    }

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::SetWindowFontScale(1.2f);
    ImGui::Text("Total:"); ImGui::SameLine();
    ImGui::TextColored(COL_GOLD, "$%.2f", CartTotal());
    ImGui::SetWindowFontScale(1.0f);

    ImGui::Spacing();
    ImGui::Text("Payment:");
    ImGui::SameLine();

    if (payMethod == 0)
        ImGui::PushStyleColor(ImGuiCol_Button,
            ImVec4(0.25f, 0.40f, 0.20f, 1.f));
    else
        ImGui::PushStyleColor(ImGuiCol_Button,
            ImVec4(0.15f, 0.15f, 0.20f, 1.f));
    if (ImGui::Button("  Cash  ", ImVec2(95, 28))) payMethod = 0;
    ImGui::PopStyleColor();
    ImGui::SameLine(0, 8);

    if (payMethod == 1)
        ImGui::PushStyleColor(ImGuiCol_Button,
            ImVec4(0.20f, 0.25f, 0.45f, 1.f));
    else
        ImGui::PushStyleColor(ImGuiCol_Button,
            ImVec4(0.15f, 0.15f, 0.20f, 1.f));
    if (ImGui::Button("  Card  ", ImVec2(95, 28))) payMethod = 1;
    ImGui::PopStyleColor();

    ImGui::Spacing(); ImGui::Spacing();

    static string coErr;
    ImGui::PushStyleColor(ImGuiCol_Button,
        ImVec4(0.38f, 0.30f, 0.72f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
        ImVec4(0.50f, 0.42f, 0.90f, 1.f));
    if (ImGui::Button("  Checkout ->  ", ImVec2(200, 40))) {
        coErr.clear();
        if (cartEntries.empty()) {
            coErr = "Cart is empty!";
        }
        else {
            double total = CartTotal();
            int n = (int)cartEntries.size();
            int ordId = rand() % 1000 + 1000;

            CartItem** items = new CartItem * [n];
            string summary;
            for (int i = 0; i < n; i++) {
                Product* p =
                    inventory->findProduct(cartEntries[i].productId);
                if (!p) continue;
                items[i] = new CartItem(*p, cartEntries[i].qty);
                if (i > 0) summary += ", ";
                summary += to_string(cartEntries[i].qty) +
                    "x " + cartEntries[i].productName;
            }

            Order* order = new Order(ordId, items, n, total);
            PaymentMethod* pm = (payMethod == 0)
                ? (PaymentMethod*)new CashPayment()
                : (PaymentMethod*)new CardPayment();
            order->processOrder(pm);

            for (auto& ce : cartEntries)
                inventory->updateStock(ce.productId, -ce.qty);

            string payLabel = payMethod == 0 ? "Cash" : "Card";
            lastOrder = { ordId, summary, total, payLabel, NowTime() };
            orderHistory.push_back(lastOrder);
            AddNotification("Order #" + to_string(ordId) +
                " placed!", 1);

            delete pm; delete order;
            for (int i = 0; i < n; i++) delete items[i];
            delete[] items;

            cartEntries.clear();
            checkoutSuccess = true;
        }
    }
    ImGui::PopStyleColor(2);
    if (!coErr.empty())
        ImGui::TextColored(COL_RED, " %s", coErr.c_str());
}

void StockFlowApp::RenderOrderHistoryTab()
{
    ImGui::Spacing();
    if (orderHistory.empty()) {
        ImGui::TextColored(COL_MUTED, "  No orders yet.");
        return;
    }

    ImGuiTableFlags flags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_ScrollY;

    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::BeginTable("##ord", 5, flags,
        ImVec2(-1, io.DisplaySize.y - 300)))
    {
        ImGui::TableSetupColumn("Order#", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Items", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Total", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableSetupColumn("Payment", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableHeadersRow();

        for (int i = (int)orderHistory.size() - 1; i >= 0; i--) {
            auto& o = orderHistory[i];
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(COL_ACCENT, "#%d", o.orderId);
            ImGui::TableSetColumnIndex(1);
            ImGui::TextWrapped("%s", o.items.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::TextColored(COL_GOLD, "$%.2f", o.total);
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%s", o.payment.c_str());
            ImGui::TableSetColumnIndex(4);
            ImGui::TextColored(COL_MUTED, "%s", o.timestamp.c_str());
        }
        ImGui::EndTable();
    }
}
