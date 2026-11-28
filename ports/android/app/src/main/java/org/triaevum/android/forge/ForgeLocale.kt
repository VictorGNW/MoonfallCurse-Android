package org.triaevum.android.forge

import android.app.LocaleManager
import android.content.Context
import android.content.res.Configuration
import android.content.res.Resources
import android.os.Build
import android.os.LocaleList
import android.util.Log
import java.io.File
import java.util.Locale
import org.json.JSONObject

/** Installer interface language follows Android unless the user chooses an override. */
object ForgeLocale {
    const val ENGLISH = "en"
    val INSTALLER_LANGUAGES = listOf("en", "fr", "de", "es", "it", "pt")
    const val PREFERENCES_FORMAT = "triaevum_forge_preferences_v1"
    private const val KEY = "ui_language"
    private const val TAG = "TriAevumForge"
    private const val PREFERENCES = "installer-locale"
    private const val KEY_APPLIED = "applied"

    /** The game languages by their own names, with accents (the runtime reports ASCII labels). */
    val NATIVE_NAMES = mapOf(
        "en" to "English",
        "fr" to "Français",
        "de" to "Deutsch",
        "es" to "Español",
        "it" to "Italiano",
        "nl" to "Nederlands",
        "pt" to "Português (Brasil)",
    )

    fun choose(saved: String?, deviceLanguages: List<String>): String =
        saved?.takeIf { it in INSTALLER_LANGUAGES }
            ?: deviceLanguages.firstOrNull { it in INSTALLER_LANGUAGES }
            ?: ENGLISH

    fun systemLanguages(context: Context): List<String> {
        val locales = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            context.getSystemService(LocaleManager::class.java).systemLocales
        } else {
            Resources.getSystem().configuration.locales
        }
        return (0 until locales.size()).map { locales[it].language }
    }

    private fun settingsFile(profileRoot: File) = File(profileRoot, "data/config/forge.json")

    /** The kept choice: two lowercase letters, or null (none, or an unreadable file). */
    fun savedChoice(profileRoot: File?): String? {
        val file = profileRoot?.let(::settingsFile)?.takeIf { it.isFile } ?: return null
        return try {
            JSONObject(file.readText()).optString(KEY).takeIf { it.length == 2 && it.all { c -> c in 'a'..'z' } }
        } catch (error: Exception) {
            null
        }
    }

    /** Keeps [code] as the user's choice; the file's other members stay. False when it cannot be written. */
    fun saveChoice(profileRoot: File, code: String?): Boolean {
        val file = settingsFile(profileRoot)
        return try {
            val document = try {
                if (file.isFile) JSONObject(file.readText()) else JSONObject()
            } catch (_: Exception) {
                JSONObject()
            }
            document.put("format", PREFERENCES_FORMAT)
            if (code == null) document.remove(KEY) else document.put(KEY, code)
            file.parentFile?.mkdirs()
            val temporary = File(file.parentFile, ".forge.json.tmp")
            temporary.writeText(document.toString(2))
            if (!temporary.renameTo(file)) {
                file.delete()
                check(temporary.renameTo(file)) { "rename failed" }
            }
            true
        } catch (error: Exception) {
            Log.w(TAG, "Cannot keep the installer language", error)
            false
        }
    }

    /** The language the app's texts are in now. */
    fun current(context: Context): String = context.resources.configuration.locales[0].language

    /**
     * Applies [code] to the app. Android 13+: the per-app language, delivered to the running
     * activities as a configuration change. Older versions: kept for [wrap] and applied to
     * [context]'s resources in place. Returns whether anything changed (the caller re-renders).
     */
    fun apply(context: Context, code: String, followDevice: Boolean): Boolean {
        val locale = Locale.forLanguageTag(code)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            val manager = context.getSystemService(LocaleManager::class.java)
            val applied = manager.applicationLocales
            if (followDevice && manager.systemLocales[0].language == code) {
                if (applied.isEmpty) return false
                manager.applicationLocales = LocaleList.getEmptyLocaleList()
                return true
            }
            if (applied.size() == 1 && applied[0].language == code) return false
            Locale.setDefault(locale)
            manager.applicationLocales = LocaleList(locale)
            return true
        }
        val preferences = context.getSharedPreferences(PREFERENCES, Context.MODE_PRIVATE)
        if (preferences.getString(KEY_APPLIED, null) == code && current(context) == code) return false
        preferences.edit().putString(KEY_APPLIED, code).commit()
        Locale.setDefault(locale)
        for (resources in listOf(context.resources, context.applicationContext.resources)) {
            val configuration = Configuration(resources.configuration)
            configuration.setLocales(LocaleList(locale))
            @Suppress("DEPRECATION")
            resources.updateConfiguration(configuration, resources.displayMetrics)
        }
        return true
    }

    /**
     * `attachBaseContext` of the installer's components before Android 13: [base] in the applied
     * language (Android 13+ applies the per-app language itself).
     */
    fun wrap(base: Context): Context {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) return base
        val code = base.getSharedPreferences(PREFERENCES, Context.MODE_PRIVATE).getString(KEY_APPLIED, null) ?: return base
        val locale = Locale.forLanguageTag(code)
        Locale.setDefault(locale)
        val configuration = Configuration(base.resources.configuration)
        configuration.setLocales(LocaleList(locale))
        return base.createConfigurationContext(configuration)
    }
}
